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

#include "mod_mshield_debug.h"
#include "mod_mshield_errno.h"
#include "mod_mshield_compat.h"


/********************************************************************
 * configuration default values
 */
#define MOD_MSHIELD_COOKIE_NAME			"MOD_MSHIELD"					/* The name of the session cookie */
#define MOD_MSHIELD_COOKIE_DOMAIN			""						/* Cookie Domain Specifier */
#define MOD_MSHIELD_COOKIE_PATH			"/"						/* The path of the cookie */
#define MOD_MSHIELD_COOKIE_REFUSE_URL		"/mod_mshield/error/refused_cookies.html"		/* URL, if client refuses the set-cookie header and if not configured in httpd.conf */
#define MOD_MSHIELD_SESSION_FREE_URL		"^/robots\\.txt$|^/favicon\\.ico$"		/* FREE URL's (session not required for theses regexp URL's) */
#define MOD_MSHIELD_COOKIE_EXPIRATION		""						/* The expiration date of the cookie */
#define MOD_MSHIELD_COOKIE_SECURE			1						/* Cookie secure flag (0, 1) */
#define MOD_MSHIELD_COOKIE_HTTPONLY			1						/* Cookie HTTPonly flag (0, 1) */
#define MOD_MSHIELD_DEFAULT_SHM_SIZE		"32768"						/* Default Shared Memory Segment */
#define MOD_MSHIELD_SESSION_HARD_TIMEOUT		3600						/* Session hard timeout in seconds */
#define MOD_MSHIELD_SESSION_INACTIVITY_TIMEOUT	900						/* Session inactivity timeout in seconds */
#define MOD_MSHIELD_SESSION_INACTIVITY_TIMEOUT_URL	"/mod_mshield/error/session_inactivity.html"	/* Session inactivity timeout URL */
#define MOD_MSHIELD_SESSION_TIMEOUT_URL		"/mod_mshield/error/session_expired.html"		/* Session timeout URL */
#define MOD_MSHIELD_SESSION_RENEW_URL		"^/renew/"					/* regexp when a session shall be renewed */
#define MOD_MSHIELD_SESSION_DESTROY			"^/logout/"					/* session destroy regexp */
#define MOD_MSHIELD_SESSION_DESTROY_URL		"/mod_mshield/error/session_destroy.html"		/* session destroy url */
#define MOD_MSHIELD_LOGON_SERVER_URL		"/mod_mshield/login.html"				/* URL for global logon server (default) */
#define MOD_MSHIELD_LOGON_SERVER_URL_1		"/mod_mshield/login.html"				/* URL for global logon server (username & password) */
#define MOD_MSHIELD_LOGON_SERVER_URL_2		"/mod_mshield/login.html"				/* URL for global logon server (strong authentication) */
#define MOD_MSHIELD_LOGON_AUTH_COOKIE_NAME		"LOGON"						/* Cookiename for authentication */
#define MOD_MSHIELD_LOGON_AUTH_COOKIE_VALUE		"ok"						/* Cookievalue for successful authentication */
#define MOD_MSHIELD_SHM_USED_URL			"/mod_mshield/error/session_shm_used.html"		/* URL if a shm problem occours */
#define MOD_MSHIELD_FREE_COOKIES			"^language=|^trustme="				/* cookies not stored in cookie store */
#define MOD_MSHIELD_SERVICE_LIST_COOKIE_NAME	"MOD_MSHIELD_SERVICE_LIST"				/* The name of the  cookie */
#define MOD_MSHIELD_SERVICE_LIST_COOKIE_VALUE	"^/.*$"						/* default service list */
#define MOD_MSHIELD_SERVICE_LIST_ERROR_URL		"/mod_mshield/error/authorization_error.html"	/* authorization error */
#define MOD_MSHIELD_AUTHORIZED_LOGON_URL		"^/.*$"						/* from what r->uri LOGON=ok cookies are accepted */
#define MOD_MSHIELD_URL_AFTER_RENEW			"/url_after_renew/"				/* set url after renew here */
#define MOD_MSHIELD_ENABLED_RETURN_TO_ORIG_URL	"^/.*$"						/* from what r->uri LOGON=ok cookies are accepted */


/********************************************************************
 * Compile time configuration
 */

/*
 * Session ID bytes: 192 bits of entropy is 2^64 times better security than "standard" 128 bits
 * Note that under Linux, starving entropy from /dev/random can lead to Apache blocking until
 * sufficient amounts of entropy is available.  This is an APR issue, not a mod_mshield issue.
 */
#define MOD_MSHIELD_SIDBYTES		24

/*
 * Cookie test suffix; appended to URLs like host/foo/bar?__cookie_try=1
 */
#define MOD_MSHIELD_COOKIE_TRY		"__cookie_try"

/*
 * 20000 sessions require about 30 seconds to start (init) and allocate 6 MB
 * 10000 sessions require about 10 seconds to start (init) and allocate 3 MB
 * (on a Sun E4500 Solaris 10 system with 8 400 MHz Sparc CPUs)
 *
 * These are meant to be overridden using the -D compiler/preprocessor option.
 */
#ifndef MOD_MSHIELD_SESSION_COUNT
#define MOD_MSHIELD_SESSION_COUNT		100	/* Default number of mod_mshield sessions (SHM) */
#endif
#ifndef MOD_MSHIELD_COOKIESTORE_COUNT
#define MOD_MSHIELD_COOKIESTORE_COUNT	200	/* Default cookiestore size (SHM) */
#endif


#define MOD_MSHIELD_HASH_LENGTH		100


/********************************************************************
 * module declaration
 */
extern module AP_MODULE_DECLARE_DATA mshield_module;


/********************************************************************
 * configuration structures
 */
typedef struct {
	int enabled;					/* [On, Off] switch for enable/disable mod_mshield */
	const char *client_refuses_cookies_url;		/* Error URL, if the client refuses our mod_mshield cookie */
	const char *cookie_name;			/* The cookie name value of the mod_mshield cookie */
	const char *cookie_domain;			/* The cookie domain value */
	const char *cookie_path;			/* The cookie path value */
	const char *cookie_expiration;			/* The cookie expiration flag value */
	int cookie_secure;				/* The cookie secure flag value */
	int cookie_httponly;				/* The HTTPonly flag (for MS IE only) */
	const char *session_free_url;			/* Regexp statement, for which mod_mshield is not enforced */

	apr_int64_t session_hard_timeout;		/* How long a mod_mshield session is accepted, before a new must be given */
	apr_int64_t session_inactivity_timeout;		/* How long the client can do *nothing*, before it's session expires */
	const char *session_expired_url;		/* Error URL, once a session times out (expires); defaults to renew URL XXX */
	const char *session_renew_url;			/* URL for which MOD_MSHIELD sets new MOD_MSHIELD session */

	const char *all_shm_space_used_url;		/* Error URL, if all sessions are taken by mod_mshield and NO shm available */

	const char *session_destroy;			/* Session destroy URI */
	const char *session_destroy_url;		/* Error URL, once we have destroyed the session */

	int authorization_enabled;

	const char *global_logon_server_url;		/* Logon Server URI */
	const char *global_logon_server_url_1;
	const char *global_logon_server_url_2;
	const char *global_logon_auth_cookie_name;	/* Cookie Name, which is used as authenticator */
	const char *global_logon_auth_cookie_value;	/* Cookie Value, which is used as authenticator */

	const char *session_store_free_cookies;		/* The cookies configured here are not handled by the session store */

	const char *service_list_cookie_name;		/* service list cookie name */
	const char *service_list_cookie_value;		/* service list */
	const char *service_list_error_url;		/* error, if user is not authorized */
	int service_list_enabled_on;
	const char *authorized_logon_url;		/* Regexp from what r->uri LOGON=ok are accepted */
	const char *url_after_renew;			/* Redirect URL after renew session */

	int mshield_config_enabled_return_to_orig_url;	/* IF RETURN TO ORIG URL SHALL BE ENABLED/DISABLED */

} mod_mshield_server_t;

typedef struct {
	const char *logon_server_url;			/* Logon Server URI */
	const int logon_required;			/* is logon required? */
	const int mod_mshield_location_id;			/* to group the backend sessions */
	const int mod_mshield_auth_strength;		/* required authentication strength per directory */
} mod_mshield_dir_t;


/********************************************************************
 * SHM structures
 */
/* session data */
typedef struct {
	int		slot_used;
	char		session_name[32];		/* Name of session cookie */
	char		session_id[MOD_MSHIELD_SIDBYTES/3*4+1]; /* Value of session cookie, MOD_MSHIELD_SIDBYTES random bytes, Base64 */
	char		url[255];			/* Used to store URLs for client redirection */
	int		ctime;
	int		atime;
	int		cookiestore_index;		/* index of first cookie in cookie store; -1 if none */
	int		logon_state;			/* 0 = not logged in, 1 = logged in */
	int		redirect_on_auth_flag;		/* Redirect client to orig_url on first authenticated request to protected URL */
	char		service_list[100];
	int		auth_strength;
	char		redirect_url_after_login[255];
	char 		uuid[64];
} session_data_t;
/* cookie data */
typedef struct {
	int		slot_used;
	char		name[100];
	char		value[100];
	int		next;
	int		prev;
	int		location_id;
} cookie_t;

/********************************************************************
 * Session handling API structures
 */
/* Opaque session handle type, portable across processes. */
typedef int session_handle_t;
#define INVALID_SESSION_HANDLE (-1)
/* Session type for use by callers, only valid within a single process. */
typedef struct {
	session_handle_t	handle;
	session_data_t		*data;
	request_rec		*request;
	mod_mshield_server_t	*config;
} session_t;

/********************************************************************
 * Iterator data structure (parameters and result)
 */
typedef struct {
	/* IN */
	request_rec	*r;		/* request record */
	/* only response cookie filter */
	session_t	*session;	/* session context */

	/* OUT */
	apr_status_t	status;		/* error status from callbacks */
	apr_table_t	*headers;	/* headers to add back into headers(_out|_in) */
	/* only response cookie filter */
	int		must_renew;	/* must renew session ID */
	/* only request cookie filter */
	const char	*sessionid;	/* session ID read from cookie */
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
apr_status_t mshield_add_session_cookie_to_headers(request_rec *r, mod_mshield_server_t *config, apr_table_t *headers, session_t *session);
int mshield_add_to_headers_out_cb(void *data, const char *key, const char *value);
int mshield_add_to_headers_in_cb(void *data, const char *key, const char *value);

/********************************************************************
 * mod_mshield_access_control.c
 */
apr_status_t mshield_access_control(request_rec *r, session_t *session, mod_mshield_server_t *config, mod_mshield_dir_t *dconfig);

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
apr_status_t mshield_session_create(session_t *session);
void mshield_session_unlink(session_t *session);
apr_status_t mshield_session_validate(session_t *session, int hard_timeout, int inactivity_timeout);
apr_status_t mshield_session_renew(session_t *session);
const char * mshield_session_get_cookies(session_t *session);
apr_status_t mshield_session_set_cookie(session_t *session, const char *key, const char *value, int locid);

/********************************************************************
 * mod_mshield_shm.c
 */
apr_status_t mshield_shm_initialize(apr_pool_t *p, apr_pool_t *plog, apr_pool_t *ptemp, server_rec *s);
apr_status_t shm_cleanup(void *not_used);
apr_status_t mshield_shm_initialize_cookiestore(apr_pool_t *p, apr_pool_t *plog, apr_pool_t *ptemp, server_rec *s);
apr_status_t shm_cleanup_cookiestore(void *not_used);
/* the following SHM functions are for session internal use only */
session_data_t * get_session_by_index(int index);
void mshield_shm_free(session_data_t *session_data);
int mshield_shm_timeout(session_data_t *session_data, int hard_timeout, int inactivity_timeout);
apr_status_t create_new_shm_session(request_rec *r, const char *sid, int *session_index);
const char * collect_cookies_from_cookiestore(request_rec *r, int anchor);
void mshield_cookiestore_free(int anchor);
apr_status_t store_cookie_into_session(request_rec *r, session_data_t *session_data, const char *key, const char *value, int locid);

/********************************************************************
 * mod_mshield_config.c
 */
extern const command_rec mshield_cmds[];

#endif /* MOD_MSHIELD_H */