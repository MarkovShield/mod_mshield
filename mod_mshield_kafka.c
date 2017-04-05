#include "mod_mshield.h"

/*
 * Connect to Kafka broker
 */
static apr_status_t kafka_connect(apr_pool_t *p, request_rec *r, mod_mshield_kafka_t *kafka) {
    const char *brokers = kafka->broker;

    if (!brokers || strlen(brokers) == 0) {
        brokers = MOD_MSHIELD_KAFKA_BROKER;
    }

    /* Configuration */
    rd_kafka_conf_t *conf = rd_kafka_conf_new();
    if (!conf) {
        ERRLOG_REQ_CRIT("Init Kafka conf failed");
        return APR_EINIT;
    }

    /* Quick termination */
    char tmp[512];
    snprintf(tmp, sizeof(tmp), "%i", SIGIO);
    rd_kafka_conf_set(conf, "internal.termination.signal", tmp, NULL, 0);

    /* Set configuration */
    apr_hash_index_t *hash = apr_hash_first(p, kafka->conf.global);
    while (hash) {
        const void *property = NULL;
        void *value = NULL;
        apr_hash_this(hash, &property, NULL, &value);
        if (value) {
            ERRLOG_REQ_INFO("global configration: %s = %s", (char *) property, (char *) value);

            if (rd_kafka_conf_set(conf, (char *) property, (char *) value,
                                  tmp, sizeof(tmp)) != RD_KAFKA_CONF_OK) {
                ERRLOG_REQ_INFO("Kafka config: %s", tmp);
                rd_kafka_conf_destroy(conf);
                return APR_EINIT;
            }
        }
        hash = apr_hash_next(hash);
    }

    /* Create producer handle */
    kafka->rk = rd_kafka_new(RD_KAFKA_PRODUCER, conf, NULL, 0);
    if (!kafka->rk) {
        ERRLOG_REQ_CRIT("Kafka producer init failed");
        rd_kafka_conf_destroy(conf);
        return APR_EINIT;
    }
    conf = NULL;

    ERRLOG_REQ_INFO("Created Kafka producer");

    rd_kafka_set_log_level(kafka->rk, 0);

    /* Add brokers */
    if (rd_kafka_brokers_add(kafka->rk, brokers) == 0) {
        ERRLOG_REQ_CRIT("Add Kafka brokers: %s", brokers);
        rd_kafka_destroy(kafka->rk);
        kafka->rk = NULL;
        return APR_EINIT;
    }

    ERRLOG_REQ_INFO("Add Kafka brokers: %s", brokers);

    return APR_SUCCESS;
}

/*
 * Connect to a speficied Kafka topic and save its handle
 */
static rd_kafka_topic_t *
kafka_topic_connect(apr_pool_t *p, request_rec *r, mod_mshield_kafka_t *kafka, const char *topic) {
    if (!topic || strlen(topic) == 0) {
        ERRLOG_REQ_CRIT("No such Kafka topic");
        return NULL;
    }

    if (!kafka->rk) {
        if (kafka_connect(p, r, kafka) != APR_SUCCESS) {
            ERRLOG_REQ_CRIT("kafka_connect() call was NOT successful.");
            return NULL;
        }
    }
    /* Fetch topic handle */
    rd_kafka_topic_t *rkt;
    rkt = (rd_kafka_topic_t *) kafka->rk_topic_analyse;
    if (rkt) {
        ERRLOG_REQ_INFO("Fetching topic handle: Got rkt from kafka->rk_topic_analyse");
        return rkt;
    }
    /* Configuration topic */
    rd_kafka_topic_conf_t *topic_conf = rd_kafka_topic_conf_new();
    if (!topic_conf) {
        ERRLOG_REQ_CRIT("Init Kafka topic conf failed");
        return NULL;
    }

    /* Set configuration topic */
    apr_hash_index_t *hash = apr_hash_first(p, kafka->conf.topic);
    while (hash) {
        const void *property = NULL;
        void *value = NULL;
        apr_hash_this(hash, &property, NULL, &value);
        if (value) {
            char err[512];
            ERRLOG_REQ_INFO("topic configration: %s = %s", (char *) property, (char *) value);

            if (rd_kafka_topic_conf_set(topic_conf, (char *) property, (char *) value,
                                        err, sizeof(err)) != RD_KAFKA_CONF_OK) {
                ERRLOG_REQ_CRIT("Kafka topic config: %s", err);
                rd_kafka_topic_conf_destroy(topic_conf);
                return NULL;
            }
        }
        hash = apr_hash_next(hash);
    }

    /* Create topic handle */
    rkt = rd_kafka_topic_new(kafka->rk, topic, topic_conf);
    if (!rkt) {
        ERRLOG_REQ_CRIT("Kafka topic handle creation failed!");
        rd_kafka_topic_conf_destroy(topic_conf);
        return NULL;
    }
    topic_conf = NULL;

    ERRLOG_REQ_INFO("Created Kafka topic: %s", topic);

    kafka->rk_topic_analyse = (const void *) rkt;

    return rkt;
}

/*
 * Send something to a specified topic. Partition is supported.
 * Note: Set partition to RD_KAFKA_PARTITION_UA if none is provieded.
 */
void kafka_produce(apr_pool_t *p, request_rec *r, mod_mshield_kafka_t *kafka,
                   const char *topic, int32_t partition, char *msg) {
    rd_kafka_topic_t *rkt = kafka_topic_connect(p, r, kafka, topic);
    if (rkt) {
        ERRLOG_REQ_INFO("produce: (%s:%i) %s", topic, partition, msg);

        /* Produce send */
        if (rd_kafka_produce(rkt, partition, RD_KAFKA_MSG_F_COPY,
                             msg, strlen(msg), NULL, 0, NULL) == -1) {
            ERRLOG_REQ_CRIT("Kafka produce failed! Topic: %s", topic);
            /*ERRLOG_GENERAL_ERROR(p, "Failed to produce to topic %s partition %i: %s",
                  rd_kafka_topic_name(rkt), partition,
                  rd_kafka_err2str(rd_kafka_errno2err(errno)));*/
        }

        /* Poll to handle delivery reports */
        rd_kafka_poll(kafka->rk, 10);
    } else {
        ERRLOG_REQ_CRIT("No such kafka topic: %s", topic);
    }
}

/*
 * Cleans up kafka stuff when apache is shutting down
 */
apr_status_t kafka_cleanup(void *arg) {
    server_rec *s = arg;
    mod_mshield_server_t *config;
    config = ap_get_module_config(s->module_config, &mshield_module);

    apr_pool_t *p = config->pool;
    if (!config) {
        return APR_SUCCESS;
    }

    ERRLOG_SRV_INFO("kafka_cleanup");

    apr_status_t rv;
    if ((rv = apr_global_mutex_lock(mshield_mutex)) != APR_SUCCESS) {
        return rv;
    }

    if (config->kafka.rk) {
        ERRLOG_SRV_INFO("Poll to handle delivery reports");
        rd_kafka_poll(config->kafka.rk, 0);

        ERRLOG_SRV_INFO("Wait for messages to be delivered");
        while (rd_kafka_outq_len(config->kafka.rk) > 0) {
            rd_kafka_poll(config->kafka.rk, 10);
        }

        ERRLOG_SRV_INFO("Destroy topic");
        apr_hash_index_t *hash = apr_hash_first(config->pool, config->kafka.conf.topic);
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
        rd_kafka_destroy(config->kafka.rk);
        config->kafka.rk = NULL;

        ERRLOG_SRV_INFO("Let backgournd threds clean up");
        int32_t i = 5;
        while (i-- > 0 && rd_kafka_wait_destroyed(500) == -1) { ;
        }
    }

    apr_global_mutex_unlock(mshield_mutex);

    ERRLOG_SRV_INFO("terminated cleanly");

    return APR_SUCCESS;
}
