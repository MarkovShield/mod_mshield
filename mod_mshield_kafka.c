#include "mod_mshield.h"

/****************************************************************************************************************
 * Some kafka helper functions
 *****************************************************************************************************************/

/*
 * Print the url store. This function can be used for debugging purposes.
 */
static void print_url_store(mod_mshield_server_t *config) {

    apr_hash_index_t *hi;
    const char *key;
    const char *value;

    ap_log_error(PC_LOG_DEBUG, NULL, "==== Printing URL store ====");
    ap_log_error(PC_LOG_DEBUG, NULL, "URL Store has %d entries.", apr_hash_count(config->url_store));

    for (hi = apr_hash_first(NULL, config->url_store); hi; hi = apr_hash_next(hi)) {
        apr_hash_this(hi, (const void **) &key, NULL, (void **) &value);
        ap_log_error(PC_LOG_DEBUG, NULL, "REGEX: Current Entry. KEY: %s VALUE: %s", key, value);
    }

    ap_log_error(PC_LOG_DEBUG, NULL, "==== Printing URL store end ====");

}

/*
 * Get the risk level of a URL
 */
static int get_url_risk_level(request_rec *r, const char *url) {

    mod_mshield_server_t *config;
    config = ap_get_module_config(r->server->module_config, &mshield_module);

    apr_hash_index_t *hi;
    const char *key;
    const char *value;
#ifdef DEBUG
    print_url_store(config);
#endif
    for (hi = apr_hash_first(NULL, config->url_store); hi; hi = apr_hash_next(hi)) {
        apr_hash_this(hi, (const void **) &key, NULL, (void **) &value);
        ap_log_error(PC_LOG_DEBUG, NULL, "REGEX: Current Entry. KEY: %s VALUE: %s", key, value);

        switch (mod_mshield_regexp_match(r, key, url)) {
            case STATUS_MATCH:
                ap_log_error(PC_LOG_DEBUG, NULL, "REGEX: Matched KEY: [%s] RISK_LEVEL: [%s] URL: [%s]", key, value,
                             url);
                return atoi(value);
            case STATUS_NOMATCH:
                ap_log_error(PC_LOG_DEBUG, NULL, "REGEX: NOT matched KEY: [%s] RISK_LEVEL: [%s] URL: [%s]", key, value,
                             url);
                break;
            case STATUS_ERROR:
            default:
                ap_log_error(PC_LOG_CRIT, NULL, "REGEX: Error happened during RegEx comparison RegEx: [%s] URL: [%s]",
                             key, url);
        }

    }
    /* Return -1 to let the caller know that its an unknown URL. */
    return -1;

}

/****************************************************************************************************************
 * Producer part
 *****************************************************************************************************************/

/*
 * Callback which will be called after each message produce
 */
static void dr_msg_cb(rd_kafka_t *rk,
                      const rd_kafka_message_t *rkmessage, void *opaque) {
    if (rkmessage->err) {
        ap_log_error(PC_LOG_DEBUG, NULL, "Message delivery failed: %s", rd_kafka_err2str(rkmessage->err));
    } else {
        ap_log_error(PC_LOG_DEBUG, NULL, "Message delivered (%zd bytes, partition %"
                PRId32
                ")\n", rkmessage->len, rkmessage->partition);
    }
}

/*
 * Connect to Kafka broker
 */
static apr_status_t kafka_connect_producer(apr_pool_t *p, mod_mshield_kafka_t *kafka) {
    const char *brokers = kafka->broker;

    if (!brokers || strlen(brokers) == 0) {
        brokers = MOD_MSHIELD_KAFKA_BROKER;
    }

    /* Configuration */
    rd_kafka_conf_t *conf = rd_kafka_conf_new();
    if (!conf) {
        ap_log_error(PC_LOG_CRIT, NULL, "Init Kafka conf failed");
        return APR_EINIT;
    }

    /* Quick termination */
    char errstr[512];
    snprintf(errstr, sizeof(errstr), "%i", SIGIO);

    /* Set configuration */
    apr_hash_index_t *hash = apr_hash_first(p, kafka->conf_producer.global);
    while (hash) {
        const void *property = NULL;
        void *value = NULL;
        apr_hash_this(hash, &property, NULL, &value);
        if (value) {
            ap_log_error(PC_LOG_DEBUG, NULL, "global producer configration: %s = %s", (char *) property,
                         (char *) value);

            if (rd_kafka_conf_set(conf, (char *) property, (char *) value,
                                  errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK) {
                ap_log_error(PC_LOG_CRIT, NULL, "Kafka config: %s", errstr);
                rd_kafka_conf_destroy(conf);
                return APR_EINIT;
            }
        }
        hash = apr_hash_next(hash);
    }

    rd_kafka_conf_set_dr_msg_cb(conf, dr_msg_cb);

    /* Create producer handle */
    kafka->rk_producer = rd_kafka_new(RD_KAFKA_PRODUCER, conf, NULL, 0);
    if (!kafka->rk_producer) {
        ap_log_error(PC_LOG_CRIT, NULL, "Kafka producer init failed");
        rd_kafka_conf_destroy(conf);
        return APR_EINIT;
    }

    rd_kafka_set_log_level(kafka->rk_producer, 7);
    //rd_kafka_set_log_level(kafka->rk_producer, 0);

    /* Add brokers */
    if (rd_kafka_brokers_add(kafka->rk_producer, brokers) == 0) {
        ap_log_error(PC_LOG_INFO, NULL, "Add Kafka brokers: %s", brokers);
        rd_kafka_destroy(kafka->rk_producer);
        kafka->rk_producer = NULL;
        return APR_EINIT;
    }

    return APR_SUCCESS;
}

/*
 * Connect to a specific Kafka topic and save its handle
 */
static rd_kafka_topic_t *
kafka_topic_connect_producer(apr_pool_t *p, mod_mshield_kafka_t *kafka, const char *topic, const char **rk_topic) {
    if (!topic || strlen(topic) == 0) {
        ap_log_error(PC_LOG_CRIT, NULL, "No such Kafka topic");
        return NULL;
    }

    if (!kafka->rk_producer) {
        if (kafka_connect_producer(p, kafka) != APR_SUCCESS) {
            ap_log_error(PC_LOG_CRIT, NULL, "kafka_connect_producer() call was NOT successful.");
            return NULL;
        }
    }

    if (!rk_topic) {
        ap_log_error(PC_LOG_CRIT, NULL, "No Kafka rk_topic provided");
        return NULL;
    }

    /* Fetch topic handle */
    rd_kafka_topic_t *rkt;
    rkt = (rd_kafka_topic_t *) *rk_topic;
    if (rkt) {
        return rkt;
    }
    /* Configuration topic */
    rd_kafka_topic_conf_t *topic_conf = rd_kafka_topic_conf_new();
    if (!topic_conf) {
        ap_log_error(PC_LOG_CRIT, NULL, "Init Kafka topic conf failed");
        return NULL;
    }

    /* Set configuration topic */
    apr_hash_index_t *hash = apr_hash_first(p, kafka->conf_producer.topic);
    while (hash) {
        const void *property = NULL;
        void *value = NULL;
        apr_hash_this(hash, &property, NULL, &value);
        if (value) {
            char err[512];
            ap_log_error(PC_LOG_DEBUG, NULL, "topic producer configration: %s = %s", (char *) property, (char *) value);

            if (rd_kafka_topic_conf_set(topic_conf, (char *) property, (char *) value,
                                        err, sizeof(err)) != RD_KAFKA_CONF_OK) {
                ap_log_error(PC_LOG_CRIT, NULL, "Kafka topic config: %s", err);
                rd_kafka_topic_conf_destroy(topic_conf);
                return NULL;
            }
        }
        hash = apr_hash_next(hash);
    }

    /* Create topic handle */
    rkt = rd_kafka_topic_new(kafka->rk_producer, topic, topic_conf);
    if (!rkt) {
        ap_log_error(PC_LOG_CRIT, NULL, "Kafka topic handle creation failed!");
        rd_kafka_topic_conf_destroy(topic_conf);
        return NULL;
    }

    *rk_topic = (const void *) rkt;

    return rkt;
}

/*
 * Send something to a specified topic. Partition is supported.
 * Note: Set partition to RD_KAFKA_PARTITION_UA if none is provided.
 */
apr_status_t kafka_produce(apr_pool_t *p, mod_mshield_kafka_t *kafka,
                           const char *topic, const char **rk_topic, int32_t partition, char *msg, const char *key) {

    struct timespec start, end;
    int64_t timeElapsed = 0;
    int served_msg = 0;
    struct timespec sleep_interval;

    sleep_interval.tv_nsec = ((kafka->delivery_check_interval % 1000) * CLOCKS_PER_SEC);
    clock_gettime(CLOCK_MONOTONIC, &start);

    rd_kafka_topic_t *rkt = kafka_topic_connect_producer(p, kafka, topic, rk_topic);
    if (rkt) {
        /* Produce send */
        if (rd_kafka_produce(rkt, partition, RD_KAFKA_MSG_F_COPY,
                             msg, strlen(msg), key, strlen(key), NULL) == -1) {
            ap_log_error(PC_LOG_CRIT, NULL, "Kafka produce failed! Topic: %s", topic);
        }
        /* Do not continue until we god the kafka delivery report which says our message was delivered successful.. */
        while (served_msg == 0) {
            served_msg = rd_kafka_poll(kafka->rk_producer, 10);
            nanosleep(&sleep_interval, NULL);
            clock_gettime(CLOCK_MONOTONIC, &end);
            timeElapsed = timespecDiff(&end, &start) / CLOCKS_PER_SEC;
            if (timeElapsed > kafka->msg_delivery_timeout) {
                ap_log_error(PC_LOG_CRIT, NULL,
                             "Kafka message delivery report not received. Timeout [%d] ms is expired [%ld] ms!. Check Kafka connection and the Kafka load.",
                             kafka->msg_delivery_timeout, (long) timeElapsed);
                return HTTP_INTERNAL_SERVER_ERROR;
            }
        }
    } else {
        ap_log_error(PC_LOG_CRIT, NULL, "No such kafka topic: %s", topic);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    return STATUS_OK;
}

/*
 * Use this function to extract some request information and send it to kafka
 * Note: We want milliseconds and not microseconds -> divide request_time by 1000.
 */
apr_status_t extract_click_to_kafka(request_rec *r, char *uuid, session_t *session) {

    mod_mshield_server_t *config;
    config = ap_get_module_config(r->server->module_config, &mshield_module);

    const char *clickUUID;
    cJSON *click_json;
    bool validationRequired;
    apr_status_t status;
    char *url = r->parsed_uri.path;
    redisContext *context = NULL;
    redisReply *reply;
    struct timeval response_timeout;

    /* For security reasons its important to remove double slashes. */
    ap_no2slash(url);

    /* Try to use UNIQUE_ID for click identification. If it's not possible to use it, generate an own unique id. */
    clickUUID = apr_table_get(r->subprocess_env, "UNIQUE_ID");
    if (clickUUID == NULL) {
        ap_log_error(PC_LOG_INFO, NULL, "Getting UNIQUE_ID was not successful.");
        clickUUID = generate_click_id(session);
    }

    click_json = cJSON_CreateObject();
    cJSON_AddItemToObject(click_json, "sessionUUID", cJSON_CreateString(uuid));
    cJSON_AddItemToObject(click_json, "clickUUID", cJSON_CreateString(clickUUID));
    cJSON_AddItemToObject(click_json, "timeStamp", cJSON_CreateNumber(r->request_time / 1000));
    cJSON_AddItemToObject(click_json, "url", cJSON_CreateString(url));

    int risk_level;
    risk_level = get_url_risk_level(r, url);
    if (risk_level != -1) {
        ap_log_error(PC_LOG_DEBUG, NULL, "URL [%s] found in url_store", url);
    } else {
        /* Default risk level for unknown urls is 0. This means they are not rated in the engine. */
        ap_log_error(PC_LOG_DEBUG, NULL, "URL [%s] NOT found in url_store", url);
        risk_level = 0;
    }

    validationRequired = risk_level >= config->fraud_detection_validation_threshold &&
                         !config->fraud_detection_learning_mode;

    cJSON_AddItemToObject(click_json, "urlRiskLevel", cJSON_CreateNumber(risk_level));
    cJSON_AddItemToObject(click_json, "validationRequired", cJSON_CreateBool(validationRequired));

    if (validationRequired) {
        context = redisConnect(config->redis.server, config->redis.port);
        if (context != NULL && context->err) {
            ap_log_error(PC_LOG_CRIT, NULL, "Error connection to redis: %s", context->errstr);
            redisFree(context);
            exit(1);
        }
        response_timeout.tv_usec = (config->redis.response_timeout * 1000);
        redisSetTimeout(context, response_timeout);
        reply = redisCommand(context, "SUBSCRIBE %s", clickUUID);
        freeReplyObject(reply);
    }

    status = kafka_produce(config->pool, &config->kafka, config->kafka.topic_analyse, &config->kafka.rk_topic_analyse,
                           RD_KAFKA_PARTITION_UA, cJSON_Print(click_json), uuid);

    cJSON_Delete(click_json);

    if (status != STATUS_OK) {
        ap_log_error(PC_LOG_CRIT, NULL, "Extract clicks to kafka was not successful.");
        return status;
    }

    /* If URL was critical, wait for a response message from the engine and parse it - but only if learning mode it not enabled. */
    if (validationRequired) {
        ap_log_error(PC_LOG_INFO, NULL, "URL [%s] risk level was [%i]", url, risk_level);
        mod_mshield_redis_cb_data_obj_t *cb_data_obj = apr_palloc(r->pool, sizeof(mod_mshield_redis_cb_data_obj_t));
        cb_data_obj->request = r;
        while(context->err != REDIS_ERR_IO && redisGetReply(context, (void**) &reply) == REDIS_OK) {
            ap_log_error(PC_LOG_INFO, NULL, "context->err is [%d] and context->errstr is [%s]", context->err, context->errstr);
            handle_mshield_result(reply, cb_data_obj);
            freeReplyObject(reply);
        }
    } else {
        status = STATUS_OK;
    }

    return status;
}

/*
 * Use this function to extract the url configurations and send it to kafka
 */
void extract_url_to_kafka(server_rec *s) {

    mod_mshield_server_t *config;
    config = ap_get_module_config(s->module_config, &mshield_module);

    cJSON *root = cJSON_CreateObject();
    apr_hash_index_t *hi;
    const char *key;
    const char *value;
    for (hi = apr_hash_first(NULL, config->url_store); hi; hi = apr_hash_next(hi)) {
        apr_hash_this(hi, (const void **) &key, NULL, (void **) &value);
        ap_log_error(PC_LOG_CRIT, NULL, "FRAUD-DETECTION: URL config. KEY: %s VALUE: %s", key, value);
        cJSON *temp;
        cJSON_AddItemToObject(root, "url_entry", temp = cJSON_CreateObject());
        cJSON_AddItemToObject(temp, "url", cJSON_CreateString(key));
        cJSON_AddItemToObject(temp, "risk_level", cJSON_CreateNumber(atoi(value)));
    }

    kafka_produce(config->pool, &config->kafka, config->kafka.topic_url_config, &config->kafka.rk_topic_url_config,
                  RD_KAFKA_PARTITION_UA, cJSON_Print(root), NULL);
    cJSON_Delete(root);
    kafka_cleanup(s);
}

/****************************************************************************************************************
 * Cleanup
 *****************************************************************************************************************/

/*
 * Cleans up kafka stuff when apache is shutting down
 */
apr_status_t kafka_cleanup(void *arg) {
    server_rec *s = arg;
    mod_mshield_server_t *config;
    config = ap_get_module_config(s->module_config, &mshield_module);

    apr_pool_t *p = config->pool;
    if (!p) {
        return APR_SUCCESS;
    }

    ERRLOG_SRV_INFO("kafka_cleanup");

    apr_status_t rv;
    if ((rv = apr_global_mutex_lock(mshield_mutex)) != APR_SUCCESS) {
        return rv;
    }

    if (config->kafka.rk_producer) {
        ERRLOG_SRV_INFO("Poll to handle delivery reports");
        rd_kafka_poll(config->kafka.rk_producer, 0);

        ERRLOG_SRV_INFO("Wait for messages to be delivered");
        while (rd_kafka_outq_len(config->kafka.rk_producer) > 0) {
            rd_kafka_poll(config->kafka.rk_producer, 10);
        }

        if (config->kafka.rk_topic_analyse) {
            ERRLOG_SRV_INFO("Destroy topic [%s]", config->kafka.rk_topic_analyse);
            rd_kafka_topic_destroy((rd_kafka_topic_t *) config->kafka.rk_topic_analyse);
        }
        if (config->kafka.rk_topic_usermapping) {
            ERRLOG_SRV_INFO("Destroy topic [%s]", config->kafka.rk_topic_usermapping);
            rd_kafka_topic_destroy((rd_kafka_topic_t *) config->kafka.rk_topic_usermapping);
        }

        ERRLOG_SRV_INFO("Destroy producer handle");
        rd_kafka_destroy(config->kafka.rk_producer);
        config->kafka.rk_producer = NULL;

        ERRLOG_SRV_INFO("Let background threads clean up");
        int32_t i = 5;
        while (i-- > 0 && rd_kafka_wait_destroyed(500) == -1) { ;
        }
    }

    apr_global_mutex_unlock(mshield_mutex);

    ERRLOG_SRV_INFO("terminated cleanly");

    return APR_SUCCESS;
}
