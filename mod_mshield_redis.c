#include "mod_mshield.h"

/*
 * Helper function which calculates the time diff between two timespec structs in nanoseconds.
 */
int64_t timespecDiff(struct timespec *timeA_p, struct timespec *timeB_p) {
    return ((timeA_p->tv_sec * 1000000000) + timeA_p->tv_nsec) -
           ((timeB_p->tv_sec * 1000000000) + timeB_p->tv_nsec);
}

/*
 * Connect to redis and return a redis context.
 */
redisContext *redis_connect(mod_mshield_server_t *config) {
    if (!config) {
        ap_log_error(PC_LOG_CRIT, NULL, "No server config for redis provided ");
        exit(1);
    }

    struct timeval timeout = { 1, 500000 }; // 1.5 seconds
    redisContext *context = redisConnectWithTimeout(config->redis.server, config->redis.port, timeout);
    if (context == NULL || context->err) {
        if (context) {
            ap_log_error(PC_LOG_CRIT, NULL, "Error connection to redis: %s", context->errstr);
            redisFree(context);
        } else {
            ap_log_error(PC_LOG_CRIT, NULL, "Redis connection error: Can't allocate redis context\n");
        }
        exit(1);
    }

    return context;
}

/*
 * Callback to handle redis replies.
 */
static void handle_mshield_result(redisContext *c, void *reply, void *cb_obj) {

    redisReply *redis_reply = reply;
    mod_mshield_redis_cb_data_obj_t *cb_data_obj = (mod_mshield_redis_cb_data_obj_t *) cb_obj;

    apr_status_t status;
    mod_mshield_server_t *config;

    config = ap_get_module_config(cb_data_obj->request->server->module_config, &mshield_module);

    if (reply == NULL) {
        return;
    }

    if (redis_reply->type == REDIS_REPLY_ARRAY && redis_reply->elements == 3) {
        ap_log_error(PC_LOG_DEBUG, NULL, "Waiting for redis result for request [%s]...",
                     apr_table_get(cb_data_obj->request->subprocess_env, "UNIQUE_ID"));
        for (int j = 0; j < redis_reply->elements; j++) {
            ap_log_error(PC_LOG_DEBUG, NULL, "REDIS SUB: [%u] %s", j, redis_reply->element[j]->str);
            if (redis_reply->element[j]->str) {
                if (strcmp(redis_reply->element[j]->str, MOD_MSHIELD_RESULT_FRAUD) == 0) {
                    ap_log_error(PC_LOG_INFO, NULL, "ENGINE RESULT: %s", MOD_MSHIELD_RESULT_FRAUD);
                    status = mod_mshield_redirect_to_relurl(cb_data_obj->request, config->fraud_detected_url);
                    if (status != HTTP_MOVED_TEMPORARILY) {
                        ap_log_error(PC_LOG_CRIT, NULL, "Redirection to fraud_detected_url failed.");
                    } else {
                        ap_log_error(PC_LOG_DEBUG, NULL, "Redirection to fraud_detected_url was successful.");
                    }
                }
                if (strcmp(redis_reply->element[j]->str, MOD_MSHIELD_RESULT_SUSPICIOUS) == 0) {
                    ap_log_error(PC_LOG_INFO, NULL, "ENGINE RESULT: %s", MOD_MSHIELD_RESULT_SUSPICIOUS);
                    status = mod_mshield_redirect_to_relurl(cb_data_obj->request, config->global_logon_server_url_1);
                    if (status != HTTP_MOVED_TEMPORARILY) {
                        ap_log_error(PC_LOG_CRIT, NULL, "Redirection to global_logon_server_url_1 failed.");
                    } else {
                        ap_log_error(PC_LOG_DEBUG, NULL, "Redirection to global_logon_server_url_1 was successful.");
                    }
                }
                if (strcmp(redis_reply->element[j]->str, MOD_MSHIELD_RESULT_OK) == 0) {
                    ap_log_error(PC_LOG_INFO, NULL, "ENGINE RESULT: %s", MOD_MSHIELD_RESULT_OK);
                }
            }

        }
    }
}

/*
 * Subscribe to a redis channel (with channel ID = clickUUID)
 */
apr_status_t redis_subscribe(apr_pool_t *p, request_rec *r, const char *clickUUID) {

    mod_mshield_server_t *config;
    config = ap_get_module_config(r->server->module_config, &mshield_module);
    apr_status_t status;

    struct timespec start, end;
    int64_t timeElapsed = 0;

    ap_log_error(PC_LOG_DEBUG, NULL, "===== Waiting for engine rating =====");
    clock_gettime(CLOCK_MONOTONIC, &start);

    redisReply *reply;
    redisContext *context = redis_connect(config);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = (config->redis.response_query_interval * 1000);

    while (true) {

        clock_gettime(CLOCK_MONOTONIC, &end);
        timeElapsed = timespecDiff(&end, &start) / CLOCKS_PER_SEC;

        redisSetTimeout(context, timeout);
        reply = redisCommand(context, "SUBSCRIBE %s", clickUUID);

        mod_mshield_redis_cb_data_obj_t *cb_data_obj = NULL;
        cb_data_obj = apr_palloc(p, sizeof(mod_mshield_redis_cb_data_obj_t));
        cb_data_obj->request = r;

        handle_mshield_result(context, reply, cb_data_obj);
        freeReplyObject(reply);

        if (timeElapsed > config->redis.response_timeout) {
            ap_log_error(PC_LOG_CRIT, NULL, "Received no message from redis. Timeout %ld ms is expired!", (long)timeElapsed);
            status = mod_mshield_redirect_to_relurl(r, config->fraud_error_url);
            if (status != HTTP_MOVED_TEMPORARILY) {
                ap_log_error(PC_LOG_CRIT, NULL, "Redirection to fraud_error_url failed.");
            } else {
                ap_log_error(PC_LOG_DEBUG, NULL, "Redirection to fraud_error_url was successful.");
            }
            break;
        }
    }

    redisFree(context);
    ap_log_error(PC_LOG_DEBUG, NULL, "===== Waiting for engine rating ended =====");

    if (apr_table_get(r->err_headers_out, "Location")) {
        return HTTP_MOVED_TEMPORARILY;
    }
    return STATUS_OK;
}
