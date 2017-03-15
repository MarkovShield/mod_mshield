/* $Id: mod_but_cookie.c 147 2010-05-30 20:28:01Z ibuetler $ */

#include "mod_but.h"

/*
 * Builds the session cookie from the given session ID and configuration.
 * String is allocated from r->pool.
 */
static const char *
but_build_session_cookie(request_rec *r, mod_but_server_t *config, char *sid)
{
	const char *cookie = NULL;
	const char *cookiename = config->cookie_name;
	const char *domain = "";
	const char *path = "";
	const char *secure = "";
	const char *httponly = "";
	const char *expiration = "";

	if (apr_strnatcmp(config->cookie_domain, "")) {
		domain = apr_psprintf(r->pool, "domain=%s; ", config->cookie_domain);
	}

	if (apr_strnatcmp(config->cookie_path, "")) {
		path = apr_psprintf(r->pool, "path=%s; ", config->cookie_path);
	}

	if (config->cookie_secure == 1) {
		secure = "secure; ";
	}

	if (config->cookie_httponly == 1) {
		httponly = "HttpOnly";
	}

	if (apr_strnatcmp(config->cookie_expiration, "")) {
		expiration = apr_psprintf(r->pool, "expires=%s; ", config->cookie_expiration);
	}

	cookie = apr_psprintf(r->pool, "%s=%s; %s%s%s%s%s", cookiename, sid, domain, path, expiration, secure, httponly);
	return cookie;
}


/*
 * Generate a new session in our session store and set a new
 * cookie containing the new session ID into headers.
 */
apr_status_t
but_add_session_cookie_to_headers(request_rec *r, mod_but_server_t *config, apr_table_t *headers, session_t *session)
{
	const char *cookiestr;

	cookiestr = but_build_session_cookie(r, config, session->data->session_id);
	if (!cookiestr) {
		return STATUS_ERROR;
	}

	char salt[] = "13371337";
	unsigned char hashedCookie[MOD_BUT_HASH_LENGTH];
	apr_cpystrn(hashedCookie, cookiestr, sizeof(cookiestr));
	ERRLOG_CRIT("FRAUD_DETECTION COOKIESTR Set-Cookie [%s]", cookiestr);
	ERRLOG_CRIT("FRAUD_DETECTION HASHED-COOKIE Set-Cookie [%s]", &hashedCookie);
	apr_table_setn(headers, "Set-Cookie", cookiestr);
	return STATUS_OK;
}

/* apr table iteration callback which adds all table elements to
 * the r->headers_out table. */
int
but_add_to_headers_out_cb(void *data, const char *key, const char *value) {
	apr_table_add(((request_rec*)data)->headers_out, key, value);
	return TRUE;
}

/* apr table iteration callback which adds all table elements to
 * the r->headers_in table. */
int
but_add_to_headers_in_cb(void *data, const char *key, const char *value) {
	apr_table_add(((request_rec*)data)->headers_in, key, value);
	return TRUE;
}
