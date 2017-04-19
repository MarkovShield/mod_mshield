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
static void handle_mshield_result(redisAsyncContext *c, void *reply, void *privdata) {
    redisReply *r = reply;
    if (reply == NULL) {
        return;
    }
    if (r->type == REDIS_REPLY_ARRAY) {
        for (int j = 0; j < r->elements; j++) {

            /*
            * ToDo Philip:
            * - Redirect to conf->fraud_detected_url if analyse result is FRAUD.
            * - Redirect to step up if analyse result is SUSPICIOUS.
            * ToDo Philip: Add event_base_loopbreak(base) somewhere here.
            */

            ap_log_error(PC_LOG_INFO, NULL, "REDIS SUB: [%u] %s", j, r->element[j]->str);
        }
    }
    freeReplyObject(reply);
}

/*
 * Subscribe to a redis channel (with channel ID = clickUUID)
 */
apr_status_t redis_subscribe(apr_pool_t *p, request_rec *r, const char *clickUUID) {

    mod_mshield_server_t *config;
    config = ap_get_module_config(r->server->module_config, &mshield_module);

    struct timespec start, end;
    int64_t timeElapsed = 1;

    struct event_base *base = event_base_new();
    redisAsyncContext *context = redis_connect(config);
    redisLibeventAttach(context, base);
    redisAsyncCommand(context, handle_mshield_result, NULL, "SUBSCRIBE %s", clickUUID);

    // ToDo Philip: Do the application logic here:
    ap_log_error(PC_LOG_INFO, NULL, "===== Starting consuming messages =====");
    clock_gettime(CLOCK_MONOTONIC, &start);
    //while ((timeElapsed / CLOCKS_PER_SEC) < config->kafka.response_timeout && msgcount != 1) {

        // ToDo Philip: Do some waiting here to slow down the busy loop.
        clock_gettime(CLOCK_MONOTONIC, &end);
        timeElapsed = timespecDiff(&end, &start);
    //}
    //redisAsyncCommand(context, handle_mshield_result, NULL, "UNSUBSCRIBE %s", clickUUID);
    //redisAsyncDisconnect(context);
    ap_log_error(PC_LOG_INFO, NULL, "Received no message from redis. Timeout [%lld] ms is expired!",
                 timeElapsed / CLOCKS_PER_SEC);

    event_base_dispatch(base);
    event_base_free(base);
    redisAsyncFree(context);

    ap_log_error(PC_LOG_INFO, NULL, "===== Stopping consuming messages =====");
    return STATUS_OK;
}

