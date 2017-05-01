/**
 * @file mod_mshield_redis.c
 * @author Philip Schmid
 * @date 1. May 2017
 * @brief File containing mod_mshield Redis related code.
 */

#include "mod_mshield.h"

/*
 * Helper function which calculates the time diff between two timespec structs in nanoseconds.
 */
int64_t timespecDiff(struct timespec *timeA_p, struct timespec *timeB_p) {
    return ((timeA_p->tv_sec * 1000000000) + timeA_p->tv_nsec) -
           ((timeB_p->tv_sec * 1000000000) + timeB_p->tv_nsec);
}

/*
 * Callback to handle redis replies.
 */
apr_status_t handle_mshield_result(void *reply, void *cb_obj) {

    redisReply *redis_reply = reply;
    mod_mshield_redis_cb_data_obj_t *cb_data_obj = (mod_mshield_redis_cb_data_obj_t *) cb_obj;

    apr_status_t status;
    mod_mshield_server_t *config;

    config = ap_get_module_config(cb_data_obj->request->server->module_config, &mshield_module);

    if (reply == NULL) {
        return STATUS_ERROR;
    }

    if (redis_reply->type == REDIS_REPLY_ARRAY && redis_reply->elements == 3) {
        ap_log_error(PC_LOG_INFO, NULL, "Waiting for redis result for request [%s]...",
                     apr_table_get(cb_data_obj->request->subprocess_env, "UNIQUE_ID"));
        for (int j = 0; j < redis_reply->elements; j++) {
            ap_log_error(PC_LOG_INFO, NULL, "REDIS SUB: [%u] %s", j, redis_reply->element[j]->str);
            if (redis_reply->element[j]->str) {
                if (strcmp(redis_reply->element[j]->str, MOD_MSHIELD_RESULT_FRAUD) == 0) {
                    ap_log_error(PC_LOG_INFO, NULL, "ENGINE RESULT: %s", MOD_MSHIELD_RESULT_FRAUD);
                    status = mod_mshield_redirect_to_relurl(cb_data_obj->request, config->fraud_detected_url);
                    if (status != HTTP_MOVED_TEMPORARILY) {
                        ap_log_error(PC_LOG_CRIT, NULL, "Redirection to fraud_detected_url failed.");
                        return HTTP_INTERNAL_SERVER_ERROR;
                    } else {
                        ap_log_error(PC_LOG_DEBUG, NULL, "Redirection to fraud_detected_url was successful.");
                        return STATUS_OK;
                    }
                }
                if (strcmp(redis_reply->element[j]->str, MOD_MSHIELD_RESULT_SUSPICIOUS) == 0) {
                    ap_log_error(PC_LOG_INFO, NULL, "ENGINE RESULT: %s", MOD_MSHIELD_RESULT_SUSPICIOUS);
                    status = mod_mshield_redirect_to_relurl(cb_data_obj->request, config->global_logon_server_url_1);
                    if (status != HTTP_MOVED_TEMPORARILY) {
                        ap_log_error(PC_LOG_CRIT, NULL, "Redirection to global_logon_server_url_1 failed.");
                        return HTTP_INTERNAL_SERVER_ERROR;
                    } else {
                        ap_log_error(PC_LOG_DEBUG, NULL, "Redirection to global_logon_server_url_1 was successful.");
                        return STATUS_OK;
                    }
                }
                if (strcmp(redis_reply->element[j]->str, MOD_MSHIELD_RESULT_OK) == 0) {
                    ap_log_error(PC_LOG_INFO, NULL, "ENGINE RESULT: %s", MOD_MSHIELD_RESULT_OK);
                    return STATUS_OK;
                }
            }

        }
    }
    return STATUS_ERROR;
}

/*
 * Subscribe to a redis channel (with channel ID = clickUUID)
 */
apr_status_t redis_subscribe(apr_pool_t *p, request_rec *r, struct event_base *base, mod_mshield_server_t *config, redisContext *context) {

    apr_status_t status;

    struct timespec start, end;
    int64_t timeElapsed = 0;

    ap_log_error(PC_LOG_DEBUG, NULL, "===== Waiting for engine rating =====");
    clock_gettime(CLOCK_MONOTONIC, &start);



    struct timeval timeout;
    timeout.tv_usec = (config->redis.response_query_interval * 1000);

    while (true) {
        event_base_loopexit(base, &timeout);
        int result = event_base_dispatch(base);
        clock_gettime(CLOCK_MONOTONIC, &end);
        timeElapsed = timespecDiff(&end, &start) / CLOCKS_PER_SEC;
        if (result < 0) {
            ap_log_error(PC_LOG_CRIT, NULL, "Error occurred while looping event_base_loop.");
        } else if (result == 1) {
            ap_log_error(PC_LOG_CRIT, NULL, "No events were pending or active.");
            continue;
        } else if (event_base_got_break(base)) {
            ap_log_error(PC_LOG_DEBUG, NULL, "Leaving event_base_dispatch because engine result was received.");
            break;
        }
        if (timeElapsed > config->redis.response_timeout) {
            ap_log_error(PC_LOG_CRIT, NULL,
                         "Received no message from redis. Timeout [%d] ms is expired [%ld] ms!. Check Redis connection and the Redis load.",
                         config->redis.response_timeout, (long) timeElapsed);
            status = mod_mshield_redirect_to_relurl(r, config->fraud_error_url);
            if (status != HTTP_MOVED_TEMPORARILY) {
                ap_log_error(PC_LOG_CRIT, NULL, "Redirection to fraud_error_url failed.");
            } else {
                ap_log_error(PC_LOG_DEBUG, NULL, "Redirection to fraud_error_url was successful.");
            }
            break;
        }
    }

    event_base_free(base);
    redisFree(context);
    ap_log_error(PC_LOG_DEBUG, NULL, "===== Waiting for engine rating ended =====");

    if (apr_table_get(r->err_headers_out, "Location")) {
        return HTTP_MOVED_TEMPORARILY;
    }
    return STATUS_OK;
}
