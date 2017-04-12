#include "mod_mshield.h"

/****************************
 * Producer functions
 */

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
    char tmp[512];
    snprintf(tmp, sizeof(tmp), "%i", SIGIO);
    rd_kafka_conf_set(conf, "internal.termination.signal", tmp, NULL, 0);

    /* Set configuration */
    apr_hash_index_t *hash = apr_hash_first(p, kafka->conf_producer.global);
    while (hash) {
        const void *property = NULL;
        void *value = NULL;
        apr_hash_this(hash, &property, NULL, &value);
        if (value) {
            ap_log_error(PC_LOG_INFO, NULL, "global configration: %s = %s", (char *) property, (char *) value);

            if (rd_kafka_conf_set(conf, (char *) property, (char *) value,
                                  tmp, sizeof(tmp)) != RD_KAFKA_CONF_OK) {
                ap_log_error(PC_LOG_INFO, NULL, "Kafka config: %s", tmp);
                rd_kafka_conf_destroy(conf);
                return APR_EINIT;
            }
        }
        hash = apr_hash_next(hash);
    }

    /* Create producer handle */
    kafka->rk_producer = rd_kafka_new(RD_KAFKA_PRODUCER, conf, NULL, 0);
    if (!kafka->rk_producer) {
        ap_log_error(PC_LOG_CRIT, NULL, "Kafka producer init failed");
        rd_kafka_conf_destroy(conf);
        return APR_EINIT;
    }

    rd_kafka_set_log_level(kafka->rk_producer, 0);

    /* Add brokers */
    if (rd_kafka_brokers_add(kafka->rk_producer, brokers) == 0) {
        ap_log_error(PC_LOG_CRIT, NULL, "Add Kafka brokers: %s", brokers);
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
            ap_log_error(PC_LOG_INFO, NULL, "topic configration: %s = %s", (char *) property, (char *) value);

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

/****************************
 * Consumer functions
 */

/*
 * Connect to Kafka broker
 */
static apr_status_t kafka_connect_consumer(apr_pool_t *p, mod_mshield_kafka_t *kafka) {
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
    char tmp[512];
    snprintf(tmp, sizeof(tmp), "%i", SIGIO);
    rd_kafka_conf_set(conf, "internal.termination.signal", tmp, NULL, 0);

    /* Set configuration */
    apr_hash_index_t *hash = apr_hash_first(p, kafka->conf_consumer.global);
    while (hash) {
        const void *property = NULL;
        void *value = NULL;
        apr_hash_this(hash, &property, NULL, &value);
        if (value) {
            ap_log_error(PC_LOG_INFO, NULL, "global configration: %s = %s", (char *) property, (char *) value);

            if (rd_kafka_conf_set(conf, (char *) property, (char *) value,
                                  tmp, sizeof(tmp)) != RD_KAFKA_CONF_OK) {
                ap_log_error(PC_LOG_INFO, NULL, "Kafka config: %s", tmp);
                rd_kafka_conf_destroy(conf);
                return APR_EINIT;
            }
        }
        hash = apr_hash_next(hash);
    }

    /* Create consumer handle */
    kafka->rk_consumer = rd_kafka_new(RD_KAFKA_CONSUMER, conf, NULL, 0);
    if (!kafka->rk_consumer) {
        ap_log_error(PC_LOG_CRIT, NULL, "Kafka consumer init failed");
        rd_kafka_conf_destroy(conf);
        return APR_EINIT;
    }

    rd_kafka_set_log_level(kafka->rk_consumer, 0);

    /* Add brokers */
    if (rd_kafka_brokers_add(kafka->rk_consumer, brokers) == 0) {
        ap_log_error(PC_LOG_CRIT, NULL, "Add Kafka brokers: %s", brokers);
        rd_kafka_destroy(kafka->rk_consumer);
        kafka->rk_consumer = NULL;
        return APR_EINIT;
    }

    return APR_SUCCESS;
}

/*
 * Connect to a specific Kafka topic and save its handle
 */
static rd_kafka_topic_t *
kafka_topic_connect_consumer(apr_pool_t *p, mod_mshield_kafka_t *kafka, const char *topic, const char **rk_topic, int32_t partition) {
    if (!topic || strlen(topic) == 0) {
        ap_log_error(PC_LOG_CRIT, NULL, "No such Kafka topic");
        return NULL;
    }

    if (!kafka->rk_consumer) {
        if (kafka_connect_consumer(p, kafka) != APR_SUCCESS) {
            ap_log_error(PC_LOG_CRIT, NULL, "kafka_connect_consumer() call was NOT successful.");
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
    apr_hash_index_t *hash = apr_hash_first(p, kafka->conf_consumer.topic);
    while (hash) {
        const void *property = NULL;
        void *value = NULL;
        apr_hash_this(hash, &property, NULL, &value);
        if (value) {
            char err[512];
            ap_log_error(PC_LOG_INFO, NULL, "topic configration: %s = %s", (char *) property, (char *) value);

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
    rkt = rd_kafka_topic_new(kafka->rk_consumer, topic, topic_conf);
    if (!rkt) {
        ap_log_error(PC_LOG_CRIT, NULL, "Kafka topic handle creation failed!");
        rd_kafka_topic_conf_destroy(topic_conf);
        return NULL;
    }

    /* Start consuming on the topic */
    rd_kafka_consume_start(rkt, partition, RD_KAFKA_OFFSET_BEGINNING);

    *rk_topic = (const void *) rkt;

    return rkt;
}



/*
 * Send something to a specified topic. Partition is supported.
 * Note: Set partition to RD_KAFKA_PARTITION_UA if none is provided.
 */
void kafka_produce(apr_pool_t *p, mod_mshield_kafka_t *kafka,
                   const char *topic, const char **rk_topic, int32_t partition, char *msg, const char *key) {
    rd_kafka_topic_t *rkt = kafka_topic_connect_producer(p, kafka, topic, rk_topic);
    if (rkt) {
        ap_log_error(PC_LOG_INFO, NULL, "produce: (%s:%i) %s", topic, partition, msg);

        /* Produce send */
        if (rd_kafka_produce(rkt, partition, RD_KAFKA_MSG_F_COPY,
                             msg, strlen(msg), key, strlen(key), NULL) == -1) {
            ap_log_error(PC_LOG_CRIT, NULL, "Kafka produce failed! Topic: %s", topic);
        }

        /* Poll to handle delivery reports */
        rd_kafka_poll(kafka->rk_producer, 10);
    } else {
        ap_log_error(PC_LOG_CRIT, NULL, "No such kafka topic: %s", topic);
    }
}

/*
 * Receive something to a specified topic. Partition is supported.
 * Note: Set partition to RD_KAFKA_PARTITION_UA if none is provided.
 */
void kafka_consume(apr_pool_t *p, mod_mshield_kafka_t *kafka,
                   const char *topic, const char **rk_topic, int32_t partition, const char *key) {
    rd_kafka_topic_t *rkt = kafka_topic_connect_consumer(p, kafka, topic, rk_topic, partition);

    if (rkt) {
        ap_log_error(PC_LOG_CRIT, NULL, "Starting consuming messages from %s", topic);
        rd_kafka_message_t *rk_message;
        rk_message = rd_kafka_consume((rd_kafka_topic_t *)rk_topic, partition, 10000);
        // ToDo Philip: Do the application logic here. The waiting and so on.
        if (rk_message == NULL) {
            ap_log_error(PC_LOG_CRIT, NULL, "CONSUMED MESSAGE an error occurred!");
            if (rk_message->err == ETIMEDOUT) {
                ap_log_error(PC_LOG_CRIT, NULL, "CONSUMED MESSAGE timeout reached!");
            }
            if (rk_message->err == ENOENT) {
                ap_log_error(PC_LOG_CRIT, NULL, "CONSUMED MESSAGE partition is unknown!");
            }
        } else {
            ap_log_error(PC_LOG_CRIT, NULL, "CONSUMED MESSAGE [%s] with key [%s]", rk_message->payload ,rk_message->key);
        }
        rd_kafka_message_destroy(rk_message);
    } else {
        ap_log_error(PC_LOG_CRIT, NULL, "No such kafka topic: %s", topic);
    }
    
}

/*
 * Use this function to extract some request information and send it to kafka
 * Note: We want milliseconds and not microseconds -> divide request_time by 1000.
 */
void extract_click_to_kafka(request_rec *r, char *uuid) {

    mod_mshield_server_t *config;
    config = ap_get_module_config(r->server->module_config, &mshield_module);

    char *url = (char *) mshield_remove_trailing_slash(r->unparsed_uri);
    //ap_log_error(PC_LOG_CRIT, NULL, "URL befor trailing / removal: [%s]", r->parsed_uri.path);
    //ap_log_error(PC_LOG_CRIT, NULL, "URL after trailing / removal: [%s]", url);

    cJSON *click_json;
    click_json = cJSON_CreateObject();
    cJSON_AddItemToObject(click_json, "uuid", cJSON_CreateString(uuid));
    cJSON_AddItemToObject(click_json, "timeStamp", cJSON_CreateNumber(r->request_time/1000));
    cJSON_AddItemToObject(click_json, "url", cJSON_CreateString(url));

    char *risk_level = NULL;
    risk_level = (char *) apr_hash_get(config->url_store, url, APR_HASH_KEY_STRING);
    if (risk_level) {
        ap_log_error(PC_LOG_CRIT, NULL, "URL [%s] found in url_store", url);
        cJSON_AddItemToObject(click_json, "urlRiskLevel", cJSON_CreateNumber(atoi(risk_level)));
    } else {
        /* Default value for unknown urls is 0. This means they are not rated in the engine. */
        ap_log_error(PC_LOG_CRIT, NULL, "URL [%s] NOT found in url_store", url);
        cJSON_AddItemToObject(click_json, "urlRiskLevel", cJSON_CreateNumber(0));
    }

    kafka_produce(config->pool, &config->kafka, config->kafka.topic_analyse, &config->kafka.rk_topic_analyse,
                  RD_KAFKA_PARTITION_UA, cJSON_Print(click_json), uuid);

    cJSON_Delete(click_json);

    // ToDo Philip: If URL was critical, call function there to wait for response
    if (risk_level && atoi(risk_level) == 1) {
        kafka_consume(config->pool, &config->kafka, config->kafka.topic_analyse_result, &config->kafka.rk_topic_analyse_result,
                      RD_KAFKA_PARTITION_UA, "test_key");
        ap_log_error(PC_LOG_CRIT, NULL, "URL [%s] risk level was [%i]", url, atoi(apr_hash_get(config->url_store, url, APR_HASH_KEY_STRING)));
    }

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
        apr_hash_this(hi, (const void**)&key, NULL, (void**)&value);
        ap_log_error(PC_LOG_CRIT, NULL, "FRAUD-DETECTION: URL config. KEY: %s VALUE: %s", key, value);
        cJSON *temp;
        cJSON_AddItemToObject(root, "url_entry", temp = cJSON_CreateObject());
        cJSON_AddItemToObject(temp, "url", cJSON_CreateString(key));
        cJSON_AddItemToObject(temp, "risk_level", cJSON_CreateNumber(atoi(value)));
    }

    kafka_produce(config->pool, &config->kafka, config->kafka.topic_url_config, &config->kafka.rk_topic_url_config,
                  RD_KAFKA_PARTITION_UA, cJSON_Print(root), key);
    cJSON_Delete(root);
    kafka_cleanup(s);
}

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

        ERRLOG_SRV_INFO("Destroy topic");
        apr_hash_index_t *hash = apr_hash_first(config->pool, config->kafka.conf_producer.topic);
        while (hash) {
            const void *topic = NULL;
            void *rkt = NULL;
            apr_hash_this(hash, &topic, NULL, &rkt);
            if (rkt) {
                ERRLOG_SRV_INFO("kafka topic = %s", (char *) topic);
                rd_kafka_topic_destroy((rd_kafka_topic_t *) rkt);
            }
            hash = apr_hash_next(hash);
        }

        ERRLOG_SRV_INFO("Destroy producer handle");
        rd_kafka_destroy(config->kafka.rk_producer);
        config->kafka.rk_producer = NULL;

        ERRLOG_SRV_INFO("Let backgournd threds clean up");
        int32_t i = 5;
        while (i-- > 0 && rd_kafka_wait_destroyed(500) == -1) { ;
        }
    }

    apr_global_mutex_unlock(mshield_mutex);

    ERRLOG_SRV_INFO("terminated cleanly");

    return APR_SUCCESS;
}
