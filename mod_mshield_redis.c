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
redisAsyncContext *redis_connect(mod_mshield_server_t *config) {
    if (!config) {
        ap_log_error(PC_LOG_CRIT, NULL, "No server config for redis provided ");
        exit(1);
    }
    redisAsyncContext *context = redisAsyncConnect(config->redis.server, config->redis.port);
    if (context != NULL && context->err) {
        ap_log_error(PC_LOG_CRIT, NULL, "Error connection to redis: %s", context->errstr);
        redisAsyncFree(context);
        exit(1);
    }
    ap_log_error(PC_LOG_INFO, NULL, "Successfully connected to redis.");
    return context;
}

/*
 * Callback to handle redis replies.
 */
static void handle_mshield_result(redisAsyncContext *c, void *reply, void *cb_obj) {

    redisReply *redis_reply = reply;
    mod_mshield_redis_cb_data_obj_t *cb_data_obj = (mod_mshield_redis_cb_data_obj_t *)cb_obj;

    apr_status_t status;
    mod_mshield_server_t *config;

    config = ap_get_module_config(cb_data_obj->request->server->module_config, &mshield_module);

    //struct timeval timeout;
    //timeout.tv_sec = 3;
    //event_base_loopexit(cb_data_obj->base, &timeout);

    if (reply == NULL) {
        return;
    }
    if (redis_reply->type == REDIS_REPLY_ARRAY && redis_reply->elements == 3) {
        for (int j = 0; j < redis_reply->elements; j++) {

            ap_log_error(PC_LOG_INFO, NULL, "REDIS SUB: [%u] %s", j, redis_reply->element[j]->str);
            if (redis_reply->element[j]->str) {
                if (strcmp(redis_reply->element[j]->str, MOD_MSHIELD_RESULT_FRAUD) == 0) {
                    ap_log_error(PC_LOG_CRIT, NULL, "ENGINE RESULT: %s", MOD_MSHIELD_RESULT_FRAUD);
                    status = mod_mshield_redirect_to_relurl(cb_data_obj->request, config->fraud_detected_url);
                    if (status != HTTP_MOVED_TEMPORARILY) {
                        ap_log_error(PC_LOG_CRIT, NULL, "Redirection to fraud_detected_url failed.");
                    }
                    event_base_loopbreak(cb_data_obj->base);
                }
                if (strcmp(redis_reply->element[j]->str, MOD_MSHIELD_RESULT_SUSPICIOUS) == 0) {
                    ap_log_error(PC_LOG_CRIT, NULL, "ENGINE RESULT: %s", MOD_MSHIELD_RESULT_SUSPICIOUS);
                    status = mod_mshield_redirect_to_relurl(cb_data_obj->request, config->global_logon_server_url_1);
                    if (status != HTTP_MOVED_TEMPORARILY) {
                        ap_log_error(PC_LOG_CRIT, NULL, "Redirection to global_logon_server_url_1 failed.");
                    }
                    event_base_loopbreak(cb_data_obj->base);
                }
                if (strcmp(redis_reply->element[j]->str, MOD_MSHIELD_RESULT_OK) == 0) {
                    ap_log_error(PC_LOG_INFO, NULL, "ENGINE RESULT: %s", MOD_MSHIELD_RESULT_OK);
                    event_base_loopbreak(cb_data_obj->base);
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

    struct timespec start, end;
    int64_t timeElapsed = 1;

    ap_log_error(PC_LOG_INFO, NULL, "===== Waiting for engine rating =====");
    clock_gettime(CLOCK_MONOTONIC, &start);

    struct event_base *base = event_base_new();
    redisAsyncContext *context = redis_connect(config);
    redisLibeventAttach(context, base);
    mod_mshield_redis_cb_data_obj_t *cb_data_obj = NULL;
    // ToDo Philip: Check this!
    cb_data_obj->base = base;
    cb_data_obj->request = r;
    redisAsyncCommand(context, handle_mshield_result, cb_data_obj, "SUBSCRIBE %s", clickUUID, r);
    //redisAsyncCommand(context, handle_mshield_result, NULL, "UNSUBSCRIBE %s", clickUUID);
    event_base_dispatch(base);
    event_base_free(base);
    redisAsyncFree(context);

    clock_gettime(CLOCK_MONOTONIC, &end);
    timeElapsed = timespecDiff(&end, &start);
    ap_log_error(PC_LOG_INFO, NULL, "Received no message from redis. Timeout [%lld] ms is expired!",
                 timeElapsed / CLOCKS_PER_SEC);

    ap_log_error(PC_LOG_INFO, NULL, "===== Waiting for engine rating ended =====");
    return STATUS_OK;
}
