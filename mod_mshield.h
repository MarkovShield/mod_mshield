/* $Id: mod_mshield.h 147 2010-05-30 20:28:01Z ibuetler $ */

#ifndef MOD_MSHIELD_H
#define MOD_MSHIELD_H

#include "apr.h"
#include "apr_lib.h"
#include "apr_strings.h"
#include "apr_buckets.h"
#include "apr_hash.h"
#include "apr_want.h"
#include "apr_shm.h"
#include "apr_rmm.h"
#include "httpd.h"
#include "http_config.h"
#include "http_request.h"
#include "http_log.h"
#include "util_filter.h"
#include "util_script.h"
#include "http_protocol.h"
#include "http_main.h"
#include "http_core.h"
#include "util_md5.h"
#include "pcre.h"
#include "apr_errno.h"
#include "apr_general.h"
#include "apr_time.h"
#include "ap_config.h"
#include "apr_optional.h"
#include "apr_base64.h"
#include "apr_anylock.h"
#include "ap_mpm.h"
#include "stdbool.h"
#include "event.h"

#include "mod_mshield_debug.h"
#include "mod_mshield_errno.h"
#include "mod_mshield_compat.h"

#include "cJSON.h"

#include "librdkafka/rdkafka.h"

#include "hiredis/hiredis.h"
#include "hiredis/async.h"
#include "hiredis/adapters/libevent.h"


/********************************************************************
 * configuration default values
 */
#define MOD_MSHIELD_COOKIE_NAME                    "MOD_MSHIELD"                                /* The name of the session cookie */
#define MOD_MSHIELD_COOKIE_DOMAIN                ""                                            /* Cookie Domain Specifier */
#define MOD_MSHIELD_COOKIE_PATH                    "/"                                            /* The path of the cookie */
#define MOD_MSHIELD_COOKIE_REFUSE_URL            "/mod_mshield/error/refused_cookies.html"   /* URL, if client refuses the set-cookie header and if not configured in httpd.conf */
#define MOD_MSHIELD_SESSION_FREE_URL            "^/robots\\.txt$|^/favicon\\.ico$"            /* FREE URL's (session not required for theses regexp URL's) */
#define MOD_MSHIELD_COOKIE_EXPIRATION            ""                                            /* The expiration date of the cookie */
#define MOD_MSHIELD_COOKIE_SECURE                1                                            /* Cookie secure flag (0, 1) */
#define MOD_MSHIELD_COOKIE_HTTPONLY                1                                            /* Cookie HTTPonly flag (0, 1) */
#define MOD_MSHIELD_DEFAULT_SHM_SIZE            "32768"                                        /* Default Shared Memory Segment */
#define MOD_MSHIELD_SESSION_HARD_TIMEOUT        3600                                        /* Session hard timeout in seconds */
#define MOD_MSHIELD_SESSION_INACTIVITY_TIMEOUT    900                                            /* Session inactivity timeout in seconds */
#define MOD_MSHIELD_SESSION_INACTIVITY_TIMEOUT_URL    "/mod_mshield/error/session_inactivity.html"    /* Session inactivity timeout URL */
#define MOD_MSHIELD_SESSION_TIMEOUT_URL            "/mod_mshield/error/session_expired.html"    /* Session timeout URL */
#define MOD_MSHIELD_SESSION_RENEW_URL            "^/renew/"                                    /* regexp when a session shall be renewed */
#define MOD_MSHIELD_SESSION_DESTROY                "^/logout/"                                    /* session destroy regexp */
#define MOD_MSHIELD_SESSION_DESTROY_URL            "/mod_mshield/error/session_destroy.html"    /* session destroy url */
#define MOD_MSHIELD_LOGON_SERVER_URL            "/mod_mshield/login.html"                    /* URL for global logon server (default) */
#define MOD_MSHIELD_LOGON_SERVER_URL_1            "/mod_mshield/login.html"                    /* URL for global logon server (username & password) */
#define MOD_MSHIELD_LOGON_SERVER_URL_2            "/mod_mshield/login.html"                    /* URL for global logon server (strong authentication) */
#define MOD_MSHIELD_LOGON_AUTH_COOKIE_NAME        "LOGON"                                        /* Cookiename for authentication */
#define MOD_MSHIELD_LOGON_AUTH_COOKIE_VALUE        "ok"                                        /* Cookievalue for successful authentication */
#define MOD_MSHIELD_SHM_USED_URL                "/mod_mshield/error/session_shm_used.html"    /* URL if a shm problem occours */
#define MOD_MSHIELD_FREE_COOKIES                "^language=|^trustme="                        /* cookies not stored in cookie store */
#define MOD_MSHIELD_SERVICE_LIST_COOKIE_NAME    "MOD_MSHIELD_SERVICE_LIST"                    /* The name of the  cookie */
#define MOD_MSHIELD_SERVICE_LIST_COOKIE_VALUE    "^/.*$"                                        /* default service list */
#define MOD_MSHIELD_SERVICE_LIST_ERROR_URL        "/mod_mshield/error/authorization_error.html"    /* authorization error */
#define MOD_MSHIELD_AUTHORIZED_LOGON_URL        "^/.*$"                                        /* from what r->uri LOGON=ok cookies are accepted */
#define MOD_MSHIELD_URL_AFTER_RENEW                "/url_after_renew/"                            /* set url after renew here */
#define MOD_MSHIELD_ENABLED_RETURN_TO_ORIG_URL    "^/.*$"                                        /* from what r->uri LOGON=ok cookies are accepted */
#define MOD_MSHIELD_USERNAME_VALUE                    "MOD_MSHIELD_USERNAME"                        /* login server LOGON cookie username value name (needs to be the same as in login.php) */
/* Fraud detection stuff starts here */
#define MOD_MSHIELD_FRAUD_DETECTION_ENABLED        0                                            /* by default the fraud detection functionality is off */
#define MOD_MSHIELD_FRAUD_LEARNING_MODE            0                                            /* by default the fraud detection learning mode is off */
#define MOD_MSHIELD_FRAUD_VALIDATION_THRESHOLD     3                                            /* If a risk level surpass or equals this threshold, a session ration result from the engine is required. */
#define MOD_MSHIELD_FRAUD_DETECTED_URL          "/error/fraud_detected.html"                    /* set the URL to redirect to if a fraud is found */
#define MOD_MSHIELD_FRAUD_ERROR_URL             "/error/fraud_error.html"                       /* set the URL to redirect to if the analyse fails */
#define MOD_MSHIELD_KAFKA_BROKER                "127.0.0.1:9092"                                /* set the kafka broker IP and port */
#define MOD_MSHIELD_KAFKA_TOPIC_ANALYSE         "MarkovClicks"                                  /* set Kafka topic on which clicks are sent to the engine */
#define MOD_MSHIELD_KAFKA_TOPIC_USERMAPPING     "MarkovLogins"                                  /* set Kafka topic on which the username <-> UUID mapping is sent to the engine */
#define MOD_MSHIELD_KAFKA_TOPIC_URL_CONFIG      "MarkovUrlConfigs"                              /* set Kafka topic on which the url <-> risk_level configuration is sent to the engine */
#define MOD_MSHIELD_KAFKA_RESPONSE_TIMEOUT      1000                                            /* set timeout to wait at most (in ms) to check if the Kafka message was successful delivered */
#define MOD_MSHIELD_KAFKA_DELIVERY_CHECK_INTERVAL 25                                            /* set the interval in ms to check for the message delivery report */
#define MOD_MSHIELD_REDIS_CONNECTION_TIMEOUT    1000                                            /* set Redis connect timeout in ms */
#define MOD_MSHIELD_REDIS_RESULT_QUERY_INTERVAL 25                                              /* set the interval in ms to query the request result */
#define MOD_MSHIELD_REDIS_SERVER                "127.0.0.1"                                     /* set the redis server */
#define MOD_MSHIELD_REDIS_PORT                  6379                                            /* set the redis server's port */
#define MOD_MSHIELD_REDIS_RESULT_TIMEOUT        3000                                            /* set how long to wait (in ms) for request analyse result */

/********************************************************************
 * Compile time configuration
 */

/*
 * Session ID bytes: 192 bits of entropy is 2^64 times better security than "standard" 128 bits
 * Note that under Linux, starving entropy from /dev/random can lead to Apache blocking until
 * sufficient amounts of entropy is available.  This is an APR issue, not a mod_mshield issue.
 */
#define MOD_MSHIELD_SIDBYTES        24

/*
 * Cookie test suffix; appended to URLs like host/foo/bar?__cookie_try=1
 */
#define MOD_MSHIELD_COOKIE_TRY        "__cookie_try"

/*
 * 20000 sessions require about 30 seconds to start (init) and allocate 6 MB
 * 10000 sessions require about 10 seconds to start (init) and allocate 3 MB
 * (on a Sun E4500 Solaris 10 system with 8 400 MHz Sparc CPUs)
 *
 * These are meant to be overridden using the -D compiler/preprocessor option.
 */
#ifndef MOD_MSHIELD_SESSION_COUNT
#define MOD_MSHIELD_SESSION_COUNT        100    /* Default number of mod_mshield sessions (SHM) */
#endif
#ifndef MOD_MSHIELD_COOKIESTORE_COUNT
#define MOD_MSHIELD_COOKIESTORE_COUNT    200    /* Default cookiestore size (SHM) */
#endif

/*
 * Return values from the engine rating
 */
#define MOD_MSHIELD_URL_CRITICALITY_LEVEL_MIN    0
#define MOD_MSHIELD_URL_CRITICALITY_LEVEL_MAX    5

/*
 * Return values from the engine rating
 */
#define MOD_MSHIELD_RESULT_FRAUD        "FRAUD"
#define MOD_MSHIELD_RESULT_SUSPICIOUS   "SUSPICIOUS"
#define MOD_MSHIELD_RESULT_OK           "OK"

#define MOD_MSHIELD_HASH_LENGTH        100


/********************************************************************
 * module declaration
 */
extern module AP_MODULE_DECLARE_DATA mshield_module;
extern apr_global_mutex_t *mshield_mutex;

/********************************************************************
 * configuration structures
 */

/**
 * @brief mod_mshield Kafka struct which stores the Kafka configuration.
 */
typedef struct {
    struct {
        apr_hash_t *global;
        apr_hash_t *topic;
    } conf_producer;
    const char *topic_analyse;                      /* Set the kafka topic on which clicks are sent to the engine */
    const char *rk_topic_analyse;                   /* topic_analyse handle */
    const char *topic_usermapping;                  /* Set the kafka topic on which the username <-> UUID mapping is sent */
    const char *rk_topic_usermapping;               /* topic_analyse_usermapping handle */
    const char *topic_url_config;                   /* Set the kafka topic on which the url <-> risk_level configuration is sent */
    const char *rk_topic_url_config;                /* topic_url_config handle */
    const char *broker;                             /* Set the IP of the Kafka broker */
    int delivery_check_interval;                    /* The interval in ms to check for the message delivery report */
    int msg_delivery_timeout;                           /* How long to wait at most (in ms) to check if the Kafka message was successful delivered. */
    rd_kafka_t *rk_producer;                        /* Kafka producer handle */
    rd_kafka_topic_partition_list_t *topics;        /* Kafka topics for high-level consumer */
} mod_mshield_kafka_t;

/**
 * @brief mod_mshield Redis struct which stores the Redis configuration.
 */
typedef struct {
    const char *server;                             /* Set the Redis server */
    int port;                                       /* Set the Redis port on which the host listens */
    int connection_timeout;                         /* Set Redis connect timeout in ms */
    int response_query_interval;                    /* The interval in ms to query request result */
    int response_timeout;                           /* How long to wait at most for request analyse result (in ms) */
} mod_mshield_redis_t;

/**
 * @brief mod_mshield struct which contains all global configurations
 */
typedef struct {
    int enabled;                                    /* [On, Off] switch for enable/disable mod_mshield */
    const char *client_refuses_cookies_url;         /* Error URL, if the client refuses our mod_mshield cookie */
    const char *cookie_name;                        /* The cookie name value of the mod_mshield cookie */
    const char *cookie_domain;                      /* The cookie domain value */
    const char *cookie_path;                        /* The cookie path value */
    const char *cookie_expiration;                  /* The cookie expiration flag value */
    int cookie_secure;                              /* The cookie secure flag value */
    int cookie_httponly;                            /* The HTTPonly flag (for MS IE only) */
    const char *session_free_url;                   /* Regexp statement, for which mod_mshield is not enforced */

    apr_int64_t session_hard_timeout;               /* How long a mod_mshield session is accepted, before a new must be given */
    apr_int64_t session_inactivity_timeout;         /* How long the client can do *nothing*, before it's session expires */
    const char *session_expired_url;                /* Error URL, once a session times out (expires); defaults to renew URL XXX */
    const char *session_renew_url;                  /* URL for which MOD_MSHIELD sets new MOD_MSHIELD session */

    const char *all_shm_space_used_url;             /* Error URL, if all sessions are taken by mod_mshield and NO shm available */

    const char *session_destroy;                    /* Session destroy URI */
    const char *session_destroy_url;                /* Error URL, once we have destroyed the session */

    int authorization_enabled;

    const char *global_logon_server_url;            /* Logon Server URI */
    const char *global_logon_server_url_1;
    const char *global_logon_server_url_2;
    const char *global_logon_auth_cookie_name;      /* Cookie Name, which is used as authenticator */
    const char *global_logon_auth_cookie_value;     /* Cookie Value, which is used as authenticator */

    const char *session_store_free_cookies;         /* The cookies configured here are not handled by the session store */

    const char *service_list_cookie_name;           /* service list cookie name */
    const char *service_list_cookie_value;          /* service list */
    const char *service_list_error_url;             /* error, if user is not authorized */
    int service_list_enabled_on;
    const char *authorized_logon_url;               /* Regexp from what r->uri LOGON=ok are accepted */
    const char *url_after_renew;                    /* Redirect URL after renew session */

    int mshield_config_enabled_return_to_orig_url;  /* IF RETURN TO ORIG URL SHALL BE ENABLED/DISABLED */
    const char *username_value;                           /* The username_value value */
    /* fraud detection stuff */
    apr_pool_t *pool;
    int fraud_detection_enabled;                    /* Enable or disable fraud detection functionality */
    int fraud_detection_learning_mode;              /* Enable or disable learning mode */
    int fraud_detection_validation_threshold;       /* Threshold which fixes if the session with risk level X should be rated by the engine */
    const char *fraud_detected_url;                 /* URL to redirect to if a fraud is found */
    const char *fraud_error_url;                    /* URL to redirect to if the analyse fails */
    apr_hash_t *url_store;                          /* url store for web application urls and its criticality */
    mod_mshield_kafka_t kafka;
    mod_mshield_redis_t redis;
} mod_mshield_server_t;

/**
 * @brief mod_mshield directory level configuration
 */
typedef struct {
    const char *logon_server_url;                   /* Logon Server URI */
    const int logon_required;                       /* is logon required? */
    const int mod_mshield_location_id;              /* to group the backend sessions */
    const int mod_mshield_auth_strength;            /* required authentication strength per directory */
} mod_mshield_dir_t;

/**
 * @brief Redis callback object.
 */
typedef struct {
    struct event_base *base;                   /* event base object */
    request_rec *request;                      /* Request object */
} mod_mshield_redis_cb_data_obj_t;

/**
 * @brief mod_mshield shared memory structures
 */
/* session data */
typedef struct {
    int slot_used;
    char session_name[32];                          /* Name of session cookie */
    char session_id[
            MOD_MSHIELD_SIDBYTES / 3 * 4 +
            1];      /* Value of session cookie, MOD_MSHIELD_SIDBYTES random bytes, Base64 */
    char url[255];                                  /* Used to store URLs for client redirection */
    int ctime;
    int atime;
    int cookiestore_index;                          /* index of first cookie in cookie store; -1 if none */
    int logon_state;                                /* 0 = not logged in, 1 = logged in */
    int redirect_on_auth_flag;                      /* Redirect client to orig_url on first authenticated request to protected URL */
    char service_list[100];
    int auth_strength;
    char redirect_url_after_login[255];
    char uuid[MOD_MSHIELD_SIDBYTES / 3 * 4 +
              1];
    char username[64];                              /* Username of Backend Web App */
} session_data_t;


/* cookie data */
typedef struct {
    int slot_used;
    char name[100];
    char value[100];
    int next;
    int prev;
    int location_id;
} cookie_t;

/********************************************************************
 * Session handling API structures
 */
/* Opaque session handle type, portable across processes. */
typedef int session_handle_t;
#define INVALID_SESSION_HANDLE (-1)
/* Session type for use by callers, only valid within a single process. */
typedef struct {
    session_handle_t handle;
    session_data_t *data;
    request_rec *request;
    mod_mshield_server_t *config;
} session_t;

/********************************************************************
 * Iterator data structure (parameters and result)
 */
typedef struct {
    /* IN */
    request_rec *r;            /* request record */
    /* only response cookie filter */
    session_t *session;    /* session context */

    /* OUT */
    apr_status_t status;        /* error status from callbacks */
    apr_table_t *headers;    /* headers to add back into headers(_out|_in) */
    /* only response cookie filter */
    int must_renew;    /* must renew session ID */
    /* only request cookie filter */
    const char *sessionid;    /* session ID read from cookie */
} cookie_res;

/********************************************************************
 * mod_mshield_redirect.c
 */
int mod_mshield_redirect_to_relurl(request_rec *r, const char *relurl);

int mod_mshield_redirect_to_cookie_try(request_rec *r, mod_mshield_server_t *config);

int mod_mshield_redirect_to_shm_error(request_rec *r, mod_mshield_server_t *config);

int mod_mshield_find_cookie_try(request_rec *r);

char *mod_mshield_strip_cookie_try(char *relurl);

/********************************************************************
 * mod_mshield_regexp.c
 */
apr_status_t mod_mshield_regexp_match(request_rec *r, const char *pattern, const char *subject);

apr_status_t mod_mshield_regexp_imatch(request_rec *r, const char *pattern, const char *subject);

apr_status_t mod_mshield_regexp_match_ex(request_rec *r, const char *pattern, int opts, const char *subject);

/********************************************************************
 * mod_mshield_cookie.c
 */
apr_status_t mshield_add_session_cookie_to_headers(request_rec *r, mod_mshield_server_t *config, apr_table_t *headers,
                                                   session_t *session);

int mshield_add_to_headers_out_cb(void *data, const char *key, const char *value);

int mshield_add_to_headers_in_cb(void *data, const char *key, const char *value);

/********************************************************************
 * mod_mshield_access_control.c
 */
apr_status_t
mshield_access_control(request_rec *r, session_t *session, mod_mshield_server_t *config, mod_mshield_dir_t *dconfig);

/********************************************************************
 * mod_mshield_response_filter.c
 */
int mod_mshield_filter_response_cookies_cb(void *result, const char *key, const char *value);

/********************************************************************
 * mod_mshield_request_filter.c
 */
int mod_mshield_filter_request_cookies_cb(void *result, const char *key, const char *value);

/********************************************************************
 * mod_mshield_session.c
 */
void mshield_session_init(session_t *session, request_rec *r, mod_mshield_server_t *config);

int mshield_session_isnull(session_t *session);

apr_status_t mshield_session_find(session_t *session, const char *session_name, const char *session_id);

apr_status_t mshield_session_open(session_t *session, session_handle_t handle);

apr_status_t mshield_session_create(session_t *session, bool is_new_session);

char *generate_uuid(session_t *session);

char *generate_click_id(session_t *session);

void mshield_session_unlink(session_t *session);

apr_status_t mshield_session_validate(session_t *session, int hard_timeout, int inactivity_timeout);

apr_status_t mshield_session_renew(session_t *session);

const char *mshield_session_get_cookies(session_t *session);

apr_status_t mshield_session_set_cookie(session_t *session, const char *key, const char *value, int locid);

/********************************************************************
 * mod_mshield_shm.c
 */
apr_status_t mshield_shm_initialize(apr_pool_t *p, apr_pool_t *plog, apr_pool_t *ptemp, server_rec *s);

apr_status_t shm_cleanup(void *not_used);

apr_status_t mshield_shm_initialize_cookiestore(apr_pool_t *p, apr_pool_t *plog, apr_pool_t *ptemp, server_rec *s);

apr_status_t shm_cleanup_cookiestore(void *not_used);

/* the following SHM functions are for session internal use only */
session_data_t *get_session_by_index(int index);

void mshield_shm_free(session_data_t *session_data);

int mshield_shm_timeout(session_data_t *session_data, int hard_timeout, int inactivity_timeout);

apr_status_t create_new_shm_session(request_rec *r, const char *sid, const char *uuid, int *session_index);

const char *collect_cookies_from_cookiestore(request_rec *r, int anchor);

void mshield_cookiestore_free(int anchor);

apr_status_t
store_cookie_into_session(request_rec *r, session_data_t *session_data, const char *key, const char *value, int locid);

/********************************************************************
 * mod_mshield_config.c
 */
extern const command_rec mshield_cmds[];

/********************************************************************
 * mod_mshield_kafka.c
 */
apr_status_t kafka_cleanup(void *s);
apr_status_t extract_click_to_kafka(request_rec *r, char *uuid, session_t *session);
void extract_url_to_kafka(server_rec *s);
apr_status_t kafka_produce(apr_pool_t *p, mod_mshield_kafka_t *kafka, const char *topic, const char **rk_topic,
                   int32_t partition, char *msg, const char *key);

/********************************************************************
 * mod_mshield_redis.c
 */
void handle_mshield_result(void *reply, void *cb_obj);
apr_status_t redis_subscribe(apr_pool_t *p, request_rec *r, struct event_base *base, mod_mshield_server_t *config, redisContext *context);
int64_t timespecDiff(struct timespec *timeA_p, struct timespec *timeB_p);

#endif /* MOD_MSHIELD_H */
