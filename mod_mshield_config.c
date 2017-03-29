/* $Id: mod_mshield_config.c 147 2010-05-30 20:28:01Z ibuetler $ */

#include "mod_mshield.h"

const char *
mshield_config_enabled(cmd_parms *cmd, void *dummy, int arg)
{
	/*
	 * Here, we defined the configuration defaults if the user does
	 * not set MOD_MSHIELD_* directives in httpd.conf
	 * See the mod_mshield.h for the default values
	 */
	mod_mshield_server_t *conf = ap_get_module_config(cmd->server->module_config, &mshield_module);
	conf->enabled = arg;
	conf->client_refuses_cookies_url = MOD_MSHIELD_COOKIE_REFUSE_URL;
	conf->cookie_name = MOD_MSHIELD_COOKIE_NAME;
	conf->cookie_domain = MOD_MSHIELD_COOKIE_DOMAIN;
	conf->cookie_path = MOD_MSHIELD_COOKIE_PATH;
	conf->cookie_expiration = MOD_MSHIELD_COOKIE_EXPIRATION;
	conf->cookie_secure = MOD_MSHIELD_COOKIE_SECURE;
	conf->cookie_httponly = MOD_MSHIELD_COOKIE_HTTPONLY;
	conf->session_free_url = MOD_MSHIELD_SESSION_FREE_URL;
	conf->session_hard_timeout = MOD_MSHIELD_SESSION_HARD_TIMEOUT;
	conf->session_inactivity_timeout = MOD_MSHIELD_SESSION_INACTIVITY_TIMEOUT;
	conf->session_expired_url = MOD_MSHIELD_SESSION_TIMEOUT_URL;
	conf->session_destroy = MOD_MSHIELD_SESSION_DESTROY;
	conf->session_destroy_url = MOD_MSHIELD_SESSION_DESTROY_URL;
	conf->session_renew_url = MOD_MSHIELD_SESSION_RENEW_URL;
	conf->authorization_enabled = 0;
	conf->global_logon_server_url = MOD_MSHIELD_LOGON_SERVER_URL;
	conf->global_logon_server_url_1 = MOD_MSHIELD_LOGON_SERVER_URL_1;
	conf->global_logon_server_url_2 = MOD_MSHIELD_LOGON_SERVER_URL_2;
	conf->global_logon_auth_cookie_name = MOD_MSHIELD_LOGON_AUTH_COOKIE_NAME;
	conf->global_logon_auth_cookie_value = MOD_MSHIELD_LOGON_AUTH_COOKIE_VALUE;
	conf->all_shm_space_used_url = MOD_MSHIELD_SHM_USED_URL;
	conf->session_store_free_cookies = MOD_MSHIELD_FREE_COOKIES;
	conf->service_list_cookie_name = MOD_MSHIELD_SERVICE_LIST_COOKIE_NAME;
	conf->service_list_cookie_value = MOD_MSHIELD_SERVICE_LIST_COOKIE_VALUE;
	conf->service_list_error_url = MOD_MSHIELD_SERVICE_LIST_ERROR_URL;
	conf->authorized_logon_url = MOD_MSHIELD_AUTHORIZED_LOGON_URL;
	conf->url_after_renew = MOD_MSHIELD_URL_AFTER_RENEW;
	return OK;
}

const char *
mshield_config_enabled_return_to_orig_url(cmd_parms *cmd, void *dummy, int arg)
{
        mod_mshield_server_t *conf = ap_get_module_config(cmd->server->module_config, &mshield_module);
        conf->mshield_config_enabled_return_to_orig_url = arg;
        return OK;
}


const char *
mshield_config_client_refuses_cookies_url(cmd_parms *cmd, void *dummy, const char *arg)
{
	mod_mshield_server_t *conf = ap_get_module_config(cmd->server->module_config, &mshield_module);
	if (arg) {
		conf->client_refuses_cookies_url = arg;
	}
	return OK;
}

const char *
mshield_config_cookie_name(cmd_parms *cmd, void *dummy, const char *arg)
{
	mod_mshield_server_t *conf = ap_get_module_config(cmd->server->module_config, &mshield_module);
	if (arg) {
		conf->cookie_name = arg;
	}
	return OK;
}

const char *
mshield_config_cookie_domain(cmd_parms *cmd, void *dummy, const char *arg)
{
	mod_mshield_server_t *conf = ap_get_module_config(cmd->server->module_config, &mshield_module);
	if (arg) {
		conf->cookie_domain = arg;
	}
	return OK;
}

const char *
mshield_config_cookie_path(cmd_parms *cmd, void *dummy, const char *arg)
{
	mod_mshield_server_t *conf = ap_get_module_config(cmd->server->module_config, &mshield_module);
	if (arg) {
		conf->cookie_path = arg;
	}
	return OK;
}

const char *
mshield_config_cookie_expiration(cmd_parms *cmd, void *dummy, const char *arg)
{
	mod_mshield_server_t *conf = ap_get_module_config(cmd->server->module_config, &mshield_module);
	if (arg) {
		conf->cookie_expiration = arg;
	}
	return OK;
}

const char *
mshield_config_cookie_secure(cmd_parms *cmd, void *dummy, int arg)
{
	mod_mshield_server_t *conf = ap_get_module_config(cmd->server->module_config, &mshield_module);
	conf->cookie_secure = arg;
	return OK;
}

const char *
mshield_config_cookie_httponly(cmd_parms *cmd, void *dummy, int arg)
{
	mod_mshield_server_t *conf = ap_get_module_config(cmd->server->module_config, &mshield_module);
	conf->cookie_httponly = arg;
	return OK;
}

const char *
mshield_config_session_free_url(cmd_parms *cmd, void *dummy, const char *arg)
{
	mod_mshield_server_t *conf = ap_get_module_config(cmd->server->module_config, &mshield_module);
	if (arg) {
		conf->session_free_url = arg;
	}
	return OK;
}

const char *
mshield_config_session_hard_timeout(cmd_parms *cmd, void *dummy, const char *arg)
{
	mod_mshield_server_t *conf = ap_get_module_config(cmd->server->module_config, &mshield_module);
	if (arg) {
		conf->session_hard_timeout = apr_atoi64(arg);
	}
	return OK;
}

const char *
mshield_config_session_inactivity_timeout(cmd_parms *cmd, void *dummy, const char *arg)
{
	mod_mshield_server_t *conf = ap_get_module_config(cmd->server->module_config, &mshield_module);
	if (arg) {
		conf->session_inactivity_timeout = apr_atoi64(arg);
	}
	return OK;
}

const char *
mshield_config_session_expired_url(cmd_parms *cmd, void *dummy, const char *arg)
{
	mod_mshield_server_t *conf = ap_get_module_config(cmd->server->module_config, &mshield_module);
	if (arg) {
		conf->session_expired_url = arg;
	}
	return OK;
}

const char *
mshield_config_session_renew_url(cmd_parms *cmd, void *dummy, const char *arg)
{
	mod_mshield_server_t *conf = ap_get_module_config(cmd->server->module_config, &mshield_module);
	if (arg) {
		conf->session_renew_url = arg;
	}
	return OK;
}

const char *
mshield_config_all_shm_used_url(cmd_parms *cmd, void *dummy, const char *arg)
{
	mod_mshield_server_t *conf = ap_get_module_config(cmd->server->module_config, &mshield_module);
	if (arg) {
		conf->all_shm_space_used_url = arg;
	}
	return OK;
}

const char *
mshield_config_session_destroy(cmd_parms *cmd, void *dummy, const char *arg)
{
	mod_mshield_server_t *conf = ap_get_module_config(cmd->server->module_config, &mshield_module);
	if (arg) {
		conf->session_destroy = arg;
	}
	return OK;
}

const char *
mshield_config_session_destroy_url(cmd_parms *cmd, void *dummy, const char *arg)
{
	mod_mshield_server_t *conf = ap_get_module_config(cmd->server->module_config, &mshield_module);
	if (arg) {
		conf->session_destroy_url = arg;
	}
	return OK;
}

const char *
mshield_config_authorization_enabled_on(cmd_parms *cmd, void *dummy, int arg)
{
	mod_mshield_server_t *conf = ap_get_module_config(cmd->server->module_config, &mshield_module);
	conf->authorization_enabled = arg;
	return OK;
}

const char *
mshield_config_global_logon_server_url(cmd_parms *cmd, void *dummy, const char *arg)
{
	mod_mshield_server_t *conf = ap_get_module_config(cmd->server->module_config, &mshield_module);
	if (arg) {
		conf->global_logon_server_url = arg;
	}
	return OK;
}

const char *
mshield_config_global_logon_server_url_1(cmd_parms *cmd, void *dummy, const char *arg)
{
	mod_mshield_server_t *conf = ap_get_module_config(cmd->server->module_config, &mshield_module);
	if (arg) {
		conf->global_logon_server_url_1 = arg;
	}
	return OK;
}

const char *
mshield_config_global_logon_server_url_2(cmd_parms *cmd, void *dummy, const char *arg)
{
	mod_mshield_server_t *conf = ap_get_module_config(cmd->server->module_config, &mshield_module);
	if (arg) {
		conf->global_logon_server_url_2 = arg;
	}
	return OK;
}

const char *
mshield_config_global_logon_auth_cookie_name(cmd_parms *cmd, void *dummy, const char *arg)
{
	mod_mshield_server_t *conf = ap_get_module_config(cmd->server->module_config, &mshield_module);
	if (arg) {
		conf->global_logon_auth_cookie_name = arg;
	}
	return OK;
}

const char *
mshield_config_global_logon_auth_cookie_value(cmd_parms *cmd, void *dummy, const char *arg)
{
	mod_mshield_server_t *conf = ap_get_module_config(cmd->server->module_config, &mshield_module);
	if (arg) {
		conf->global_logon_auth_cookie_value = arg;
	}
	return OK;
}

const char *
mshield_config_session_store_free_cookies(cmd_parms *cmd, void *dummy, const char *arg)
{
	mod_mshield_server_t *conf = ap_get_module_config(cmd->server->module_config, &mshield_module);
	if (arg) {
		conf->session_store_free_cookies = arg;
	}
	return OK;
}

const char *
mshield_config_service_list_cookie_name(cmd_parms *cmd, void *dummy, const char *arg)
{
	mod_mshield_server_t *conf = ap_get_module_config(cmd->server->module_config, &mshield_module);
	if (arg) {
		conf->service_list_cookie_name = arg;
	}
	return OK;
}

const char *
mshield_config_service_list_cookie_value(cmd_parms *cmd, void *dummy, const char *arg)
{
	mod_mshield_server_t *conf = ap_get_module_config(cmd->server->module_config, &mshield_module);
	if (arg) {
		conf->service_list_cookie_value = arg;
	}
	return OK;
}

const char *
mshield_config_service_list_error_url(cmd_parms *cmd, void *dummy, const char *arg)
{
	mod_mshield_server_t *conf = ap_get_module_config(cmd->server->module_config, &mshield_module);
	if (arg) {
		conf->service_list_error_url = arg;
	}
	return OK;
}

const char *
mshield_config_service_list_enabled(cmd_parms *cmd, void *dummy, int arg)
{
	mod_mshield_server_t *conf = ap_get_module_config(cmd->server->module_config, &mshield_module);
	if (arg) {
		conf->service_list_enabled_on = arg;
	}
	return OK;
}

const char *
mshield_config_authorized_logon_url(cmd_parms *cmd, void *dummy, const char *arg)
{
	mod_mshield_server_t *conf = ap_get_module_config(cmd->server->module_config, &mshield_module);
	if (arg) {
		conf->authorized_logon_url = arg;
	}
	return OK;
}

const char *
mshield_config_url_after_renew(cmd_parms *cmd, void *dummy, const char *arg)
{
        mod_mshield_server_t *conf = ap_get_module_config(cmd->server->module_config, &mshield_module);
        if (arg) {
                conf->url_after_renew = arg;
        }
        return OK;
}


const command_rec mshield_cmds[] =
{
	/* global configuration */
	AP_INIT_FLAG( "MOD_MSHIELD_ENABLED",                        mshield_config_enabled,                         NULL, RSRC_CONF, "mod_mshield is enabled"),
	AP_INIT_FLAG( "MOD_MSHIELD_ENABLED_RETURN_TO_ORIG_URL",     mshield_config_enabled_return_to_orig_url,      NULL, RSRC_CONF, "mod_mshield return to orig url is enabled"),
	AP_INIT_TAKE1("MOD_MSHIELD_CLIENT_REFUSES_COOKIES_URL",     mshield_config_client_refuses_cookies_url,      NULL, RSRC_CONF, "Configure mod_mshield Redirect 3"),
	AP_INIT_TAKE1("MOD_MSHIELD_COOKIE_NAME",                    mshield_config_cookie_name,                     NULL, RSRC_CONF, "Configure mod_mshield Cookie Name"),
	AP_INIT_TAKE1("MOD_MSHIELD_COOKIE_DOMAIN",                  mshield_config_cookie_domain,                   NULL, RSRC_CONF, "Configure mod_mshield Cookie Domain"),
	AP_INIT_TAKE1("MOD_MSHIELD_COOKIE_PATH",                    mshield_config_cookie_path,                     NULL, RSRC_CONF, "Configure mod_mshield Cookie Path"),
	AP_INIT_TAKE1("MOD_MSHIELD_COOKIE_EXPIRATION",              mshield_config_cookie_expiration,               NULL, RSRC_CONF, "Configure mod_mshield Cookie Expiration Time"),
	AP_INIT_FLAG( "MOD_MSHIELD_COOKIE_SECURE",                  mshield_config_cookie_secure,                   NULL, RSRC_CONF, "Configure mod_mshield Cookie Secure Flag"),
	AP_INIT_FLAG( "MOD_MSHIELD_COOKIE_HTTPONLY",                mshield_config_cookie_httponly,                 NULL, RSRC_CONF, "Configure mod_mshield HTTPOnly Flag"),
	AP_INIT_TAKE1("MOD_MSHIELD_SESSION_FREE_URL",               mshield_config_session_free_url,                NULL, RSRC_CONF, "Configure mod_mshield free URL's"),
	AP_INIT_TAKE1("MOD_MSHIELD_SESSION_HARD_TIMEOUT",           mshield_config_session_hard_timeout,            NULL, RSRC_CONF, "Configure session hard timeout"),
	AP_INIT_TAKE1("MOD_MSHIELD_SESSION_INACTIVITY_TIMEOUT",     mshield_config_session_inactivity_timeout,      NULL, RSRC_CONF, "Configure session inactivity timeout"),
	AP_INIT_TAKE1("MOD_MSHIELD_SESSION_TIMEOUT_URL",            mshield_config_session_expired_url,             NULL, RSRC_CONF, "Configure session expired URL"),
	AP_INIT_TAKE1("MOD_MSHIELD_SESSION_RENEW_URL",              mshield_config_session_renew_url,               NULL, RSRC_CONF, "Configure session renew URL"),
	AP_INIT_TAKE1("MOD_MSHIELD_ALL_SHM_SPACE_USED_URL",         mshield_config_all_shm_used_url,                NULL, RSRC_CONF, "Configure No more SHM URL"),
	AP_INIT_TAKE1("MOD_MSHIELD_SESSION_DESTROY",                mshield_config_session_destroy,                 NULL, RSRC_CONF, "Configure session destroy URI"),
	AP_INIT_TAKE1("MOD_MSHIELD_SESSION_DESTROY_URL",            mshield_config_session_destroy_url,             NULL, RSRC_CONF, "Configure session destroy URL"),
	AP_INIT_FLAG( "MOD_MSHIELD_AUTHORIZATION_ENABLED",          mshield_config_authorization_enabled_on,        NULL, RSRC_CONF, "Authorization is enabled"),
	AP_INIT_TAKE1("MOD_MSHIELD_GLOBAL_LOGON_SERVER_URL",        mshield_config_global_logon_server_url,         NULL, RSRC_CONF, "Configure Global Logon Server URL"),
	AP_INIT_TAKE1("MOD_MSHIELD_GLOBAL_LOGON_SERVER_URL_1",      mshield_config_global_logon_server_url_1,       NULL, RSRC_CONF, "Configure Global Logon Server URL 1"),
	AP_INIT_TAKE1("MOD_MSHIELD_GLOBAL_LOGON_SERVER_URL_2",      mshield_config_global_logon_server_url_2,       NULL, RSRC_CONF, "Configure Global Logon Server URL 2"),
	AP_INIT_TAKE1("MOD_MSHIELD_GLOBAL_LOGON_AUTH_COOKIE_NAME",  mshield_config_global_logon_auth_cookie_name,   NULL, RSRC_CONF, "Configure Global Logon Cookie Name"),
	AP_INIT_TAKE1("MOD_MSHIELD_GLOBAL_LOGON_AUTH_COOKIE_VALUE", mshield_config_global_logon_auth_cookie_value,  NULL, RSRC_CONF, "Configure Global Logon Cookie Value"),
	AP_INIT_TAKE1("MOD_MSHIELD_SESSION_STORE_FREE_COOKIES",     mshield_config_session_store_free_cookies,      NULL, RSRC_CONF, "Configure Cookies, which are not handled by the session store"),
	AP_INIT_TAKE1("MOD_MSHIELD_SERVICE_LIST_COOKIE_NAME",       mshield_config_service_list_cookie_name,        NULL, RSRC_CONF, "Configure Service List Cookie Name"),
	AP_INIT_TAKE1("MOD_MSHIELD_SERVICE_LIST_COOKIE_VALUE",      mshield_config_service_list_cookie_value,       NULL, RSRC_CONF, "Configure Cookies, which are not handled by the session store"),
	AP_INIT_TAKE1("MOD_MSHIELD_SERVICE_LIST_AUTH_ERROR_URL",    mshield_config_service_list_error_url,          NULL, RSRC_CONF, "Configure error page, if the user is not authorized for a specific request"),
	AP_INIT_FLAG( "MOD_MSHIELD_SERVICE_LIST_ENABLED",           mshield_config_service_list_enabled,            NULL, RSRC_CONF, "mod_mshield service list enabled"),
	AP_INIT_TAKE1("MOD_MSHIELD_AUTHORIZED_LOGON_URL",           mshield_config_authorized_logon_url,            NULL, RSRC_CONF, "Configure regexp url, from where you accept logon cookies"),
	AP_INIT_TAKE1("MOD_MSHIELD_URL_AFTER_RENEW",	        mshield_config_url_after_renew,		    NULL, RSRC_CONF, "Configure url after the session is renewed"),
	/* per directory/location configuration */
	AP_INIT_TAKE1("MOD_MSHIELD_LOGON_SERVER_URL", ap_set_string_slot, (void*)APR_OFFSETOF(mod_mshield_dir_t, logon_server_url),      OR_ALL, "Logon server relative URL for this directory"),
	AP_INIT_FLAG( "MOD_MSHIELD_LOGON_REQUIRED",   ap_set_flag_slot,   (void*)APR_OFFSETOF(mod_mshield_dir_t, logon_required),        OR_ALL, "Logon requred for this directory?"),
	AP_INIT_TAKE1("MOD_MSHIELD_LOCATION_ID",      ap_set_int_slot,    (void*)APR_OFFSETOF(mod_mshield_dir_t, mod_mshield_location_id),   OR_ALL, "Unique location ID for this directory"),
	AP_INIT_TAKE1("MOD_MSHIELD_AUTH_STRENGTH",    ap_set_int_slot,    (void*)APR_OFFSETOF(mod_mshield_dir_t, mod_mshield_auth_strength), OR_ALL, "Authentication strength required for this directory"),
	{ NULL }
};

