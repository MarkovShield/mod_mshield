#include "mod_mshield.h"

/*static APR_OPTIONAL_FN_TYPE(ap_log_set_writer_init) *log_writer_init;
static APR_OPTIONAL_FN_TYPE(ap_log_set_writer) *log_writer;

static ap_log_writer_init *default_log_writer_init = NULL;
static ap_log_writer *default_log_writer = NULL;*/

static apr_status_t
kafka_connect(apr_pool_t *p, request_rec *r, mod_mshield_kafka_t *kafka)
{
    const char *brokers = kafka->broker;

    if (!brokers || strlen(brokers) == 0) {
        brokers = MOD_MSHIELD_KAFKA_BROKER;
    }

    /* Configuration */
    rd_kafka_conf_t *conf = rd_kafka_conf_new();
    if (!conf) {
        ERRLOG_REQ_CRIT("Init Kafka conf");
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
            ERRLOG_REQ_CRIT("global configration: %s = %s", (char *)property, (char *)value);

            if (rd_kafka_conf_set(conf, (char *)property, (char *)value,
                                  tmp, sizeof(tmp)) != RD_KAFKA_CONF_OK) {
                ERRLOG_REQ_CRIT("Kafka config: %s", tmp);
                rd_kafka_conf_destroy(conf);
                return APR_EINIT;
            }
        }
        hash = apr_hash_next(hash);
    }

    /* Create producer handle */
    kafka->rk = rd_kafka_new(RD_KAFKA_PRODUCER, conf, NULL, 0);
    if (!kafka->rk) {
        ERRLOG_REQ_CRIT("Kafka producer init");
        rd_kafka_conf_destroy(conf);
        return APR_EINIT;
    }
    conf = NULL;

    ERRLOG_REQ_CRIT("Create Kafka producer");

    rd_kafka_set_log_level(kafka->rk, 0);

    /* Add brokers */
    if (rd_kafka_brokers_add(kafka->rk, brokers) == 0) {
        ERRLOG_REQ_CRIT("Add Kafka brokers: %s", brokers);
        rd_kafka_destroy(kafka->rk);
        kafka->rk = NULL;
        return APR_EINIT;
    }

    ERRLOG_REQ_CRIT("Add Kafka brokers: %s", brokers);

    return APR_SUCCESS;
}

static rd_kafka_topic_t *
kafka_topic_connect(apr_pool_t *p, request_rec *r, mod_mshield_kafka_t *kafka, const char *topic)
{
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
    rkt = (rd_kafka_topic_t *)kafka->rk_topic_analyse;
    if (rkt) {
        ERRLOG_REQ_CRIT("Fetching topic handle: Got5 rkt from kafka->rk_topic_analyse");
        return rkt;
    }
    /* Configuration topic */
    rd_kafka_topic_conf_t *topic_conf = rd_kafka_topic_conf_new();
    if (!topic_conf) {
        ERRLOG_REQ_CRIT("Init Kafka topic conf");
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
            ERRLOG_REQ_CRIT("topic configration: %s = %s", (char *)property, (char *)value);

            if (rd_kafka_topic_conf_set(topic_conf, (char *)property, (char *)value,
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

    ERRLOG_REQ_CRIT("Create Kafka: topic = %s", topic);

    kafka->rk_topic_analyse = (const void *)rkt;
    //apr_hash_set(kafka->topics, topic, APR_HASH_KEY_STRING, (const void *)rkt);

    return rkt;
}

/*
 * Set partition to RD_KAFKA_PARTITION_UA if none is provieded.
 */

void
kafka_produce(apr_pool_t *p, request_rec *r, mod_mshield_kafka_t *kafka,
              const char *topic, int32_t partition, char *msg)
{
    rd_kafka_topic_t *rkt = kafka_topic_connect(p, r, kafka, topic);
    ERRLOG_REQ_CRIT("kafka_topic_connect() called");
    if (rkt) {
        ERRLOG_REQ_CRIT("produce: (%s:%i) %s", topic, partition, msg);

        /* Produce send */
        if (rd_kafka_produce(rkt, partition, RD_KAFKA_MSG_F_COPY,
                             msg, strlen(msg), NULL, 0, NULL) == -1) {
           ERRLOG_REQ_CRIT("Kafka produce failed!!! Topic: %s", topic);
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
static void *
kafka_log_writer_init(apr_pool_t *p, server_rec *s, const char * name)
{
    DEBUG(p, "writer_init name = %s", name);

    if (strncasecmp(KAFKA_PREFIX, name, sizeof(KAFKA_PREFIX) - 1) != 0) {
        if (default_log_writer_init) {
            return default_log_writer_init(p, s, name);
        }
        return NULL;
    }

    kafka_log_t *log = (kafka_log_t *)apr_palloc(p, sizeof(kafka_log_t));

    log->dummy = &kafka_log_dummy[0];

    char *topic = apr_pstrdup(p, name + sizeof(KAFKA_PREFIX) - 1);
    int32_t partition = RD_KAFKA_PARTITION_UA;
    char *c;

    if (strcmp(topic, "-") == 0) {
        log->topic = NULL;
        log->partition = partition;
        return log;
    }

    if ((c = strchr(topic, '@')) != NULL) {
        uint32_t i = c - topic;
        topic[i++] = 0;
        partition = atoi(topic + i);
    }

    log->topic = topic;
    log->partition = partition;

    return log;
}

static apr_status_t
kafka_log_writer(request_rec *r, void *handle, const char **strs, int *strl,
                 int nelts, apr_size_t len)
{
    kafka_log_t *log = (kafka_log_t *)handle;
    if (log->dummy != kafka_log_dummy) {
        if (default_log_writer) {
            return default_log_writer(r, handle, strs, strl, nelts, len);
        }
        return OK;
    }

    uint32_t i;
    char *s, *msg = apr_palloc(r->pool, len + 1);
    for (i = 0, s = msg; i < nelts; ++i) {
        memcpy(s, strs[i], strl[i]);
        s += strl[i];
    }
    msg[len] = '\0';

    uint32_t start = 0;
    char *topic = log->topic;
    int32_t partition = log->partition;

    */
/* Format: kafka:TOPIC@PARTITION| *//*

    if (topic == NULL) {
        if (strncmp(msg, "kafka:", 6) != 0 || (s = strchr(msg + 6, '|')) == NULL) {
            if (default_log_writer) {
                return default_log_writer(r, handle, strs, strl, nelts, len);
            }
            return OK;
        }

        i = s - msg;
        msg[i] = 0;

        topic = msg + 6;
        start = i + 1;

        if ((s = strchr(topic, '@')) != NULL) {
            i = s - topic;
            topic[i++] = 0;
            partition = atoi(topic + i);
        }
    }

    kafka_config_server *config;
    config = ap_get_module_config(r->server->module_config, &mshield_module);
    if (APR_ANYLOCK_LOCK(&config->mutex) == APR_SUCCESS) {
        kafka_produce(r->pool, &config->kafka, topic, partition, msg + start);
        APR_ANYLOCK_UNLOCK(&config->mutex);
    }

    return OK;
}
*/

//static const char *
//kafka_set_brokers(cmd_parms *cmd, void *dummy, const char *arg)
//{
//    DEBUG(cmd->pool, "set brokers: %s", arg);
//    if (!arg || strlen(arg) == 0) {
//        return "KafkaBrokers argument must be a server[:port][,..]";
//    }
//
//    kafka_config_server *config;
//    config = ap_get_module_config(cmd->server->module_config, &mshield_module);
//    config->kafka.brokers = apr_pstrdup(cmd->pool, arg);
//
//    return NULL;
//}

/*static const char *
kafka_set_conf(cmd_parms *cmd, void *dummy, const char *arg)
{
    DEBUG(cmd->pool, "set conf: %s", arg);

    const char *type = ap_getword_conf(cmd->pool, &arg);
    const char *property = ap_getword_conf(cmd->pool, &arg);
    const char *value = ap_getword_conf(cmd->pool, &arg);

    if (!type || !property || !value
        || strlen(type) == 0 || strlen(property) == 0 || strlen(value) == 0) {
        return "KafkaConf argument must be a '[global|topic] <property> <value>'";
    }

    DEBUG(cmd->pool, "type = %s, property = %s, value = %s",
          type, property, value);

    kafka_config_server *config;
    config = ap_get_module_config(cmd->server->module_config, &mshield_module);

    if (strcmp(type, "global") == 0) {
        apr_hash_set(config->kafka.conf.global, property, APR_HASH_KEY_STRING,
                     (const void *)value);
    } else if(strcmp(type, "topic") == 0) {
        apr_hash_set(config->kafka.conf.topic, property, APR_HASH_KEY_STRING,
                     (const void *)value);
    } else {
        return "KafkaConf argument must be a '[global|topic] <property> <value>'";
    }

    return NULL;
}*/

apr_status_t
kafka_cleanup(void *arg)
{
    mod_mshield_server_t *config = (mod_mshield_server_t *)arg;
    apr_pool_t *p = config->pool;
    if (!config) {
        return APR_SUCCESS;
    }

    ERRLOG_GENERAL_DEBUG("kafka_cleanup");

    apr_status_t rv;
    if ((rv = apr_global_mutex_lock(mshield_mutex)) != APR_SUCCESS) {
        return rv;
    }

    if (config->kafka.rk) {
        ERRLOG_GENERAL_DEBUG("Poll to handle delivery reports");
        rd_kafka_poll(config->kafka.rk, 0);

        ERRLOG_GENERAL_DEBUG("Wait for messages to be delivered");
        while (rd_kafka_outq_len(config->kafka.rk) > 0) {
            rd_kafka_poll(config->kafka.rk, 10);
        }

        ERRLOG_GENERAL_DEBUG("Destroy topic");
        apr_hash_index_t *hash = apr_hash_first(config->pool, config->kafka.conf.topic);
        while (hash) {
            const void *topic = NULL;
            void *rkt = NULL;
            apr_hash_this(hash, &topic, NULL, &rkt);
            if (rkt) {
                ERRLOG_GENERAL_DEBUG("kafka topic = %s", (char *)topic);
                rd_kafka_topic_destroy((rd_kafka_topic_t *)rkt);
            }
            hash = apr_hash_next(hash);
        }

        ERRLOG_GENERAL_DEBUG("Destroy producer handle");
        rd_kafka_destroy(config->kafka.rk);
        config->kafka.rk = NULL;

        ERRLOG_GENERAL_DEBUG("Let backgournd threds clean up");
        int32_t i = 5;
        while (i-- > 0 && rd_kafka_wait_destroyed(500) == -1) {
            ;
        }
    }

    apr_global_mutex_unlock(mshield_mutex);

    ERRLOG_GENERAL_DEBUG("terminate cleanly");

    return APR_SUCCESS;
}
/*
static int
kafka_pre_config(apr_pool_t *p, apr_pool_t *plog, apr_pool_t *ptemp)
{
    if (!log_writer_init) {
        log_writer_init = APR_RETRIEVE_OPTIONAL_FN(ap_log_set_writer_init);
        log_writer = APR_RETRIEVE_OPTIONAL_FN(ap_log_set_writer);
    }

    if (!default_log_writer_init) {
        void *log = log_writer_init(kafka_log_writer_init);
        if (log != kafka_log_writer_init) {
            default_log_writer_init = log;
        }

        log = log_writer(kafka_log_writer);
        if (log != kafka_log_writer) {
            default_log_writer = log;
        }
    }

    return OK;
}*/

/*static void *
kafka_create_server_config(apr_pool_t *p, server_rec *s)
{
    kafka_config_server *config = apr_pcalloc(p, sizeof(kafka_config_server));
    if (!config) {
        return NULL;
    }

    DEBUG(p, "create_server_config");

    ap_mpm_query(AP_MPMQ_MAX_THREADS, &mpm_threads);

#if APR_HAS_THREADS
    if (mpm_threads > 1) {
        config->mutex.type = apr_anylock_threadmutex;
        apr_status_t rv = apr_thread_mutex_create(&config->mutex.lock.tm,
                                                  APR_THREAD_MUTEX_DEFAULT, p);
        if (rv != APR_SUCCESS) {
            ap_log_error(APLOG_MARK, APLOG_CRIT, rv, s, APLOGNO(00647)
                    "could not initialize kafka module mutex");
            config->mutex.type = apr_anylock_none;
        }
    }
    else
#endif
    {
        config->mutex.type = apr_anylock_none;
    }

    config->pool = p;

    config->kafka.conf.global = apr_hash_make(p);
    config->kafka.conf.topic = apr_hash_make(p);
    config->kafka.brokers = NULL;
    config->kafka.rk = NULL;
    config->kafka.topics = apr_hash_make(p);

    return config;
}*/
