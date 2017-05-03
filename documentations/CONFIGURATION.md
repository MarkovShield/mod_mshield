# Configuration

## Content
* [Basic module configuration](#markdown-header-basic-module-configuration)
* [Basic mod_mshield configuration](#markdown-header-basic-mod_mshield-configuration)
* [Configuration value explanations](#markdown-header-configuration-value-explanations)
* [URL configurations](#markdown-header-url-configurations)

## Basic module configuration
In `httpd.conf` load the module:
```bash
LoadModule mshield_module modules/mod_mshield.so

# Place your mod_mshield configuration directives in the following file:
Include conf/extra/httpd-preauth.conf
```

## Basic mod_mshield configuration
The next step is to configure mod_mshield depending on your needs. Therefore create another configuration file called `conf/extra/httpd-preauth.conf` and add the following lines:
```bash
#################################################################################
#   Web Pre-Auth using Apache Module MOD_MSHIELD
#################################################################################

MOD_MSHIELD_ENABLED                         On
MOD_MSHIELD_CLIENT_REFUSES_COOKIES_URL      /error/refused_cookies.html
MOD_MSHIELD_COOKIE_NAME                     LIVECD
MOD_MSHIELD_COOKIE_PATH                     /
MOD_MSHIELD_COOKIE_SECURE                   Off
MOD_MSHIELD_COOKIE_HTTPONLY                 Off
MOD_MSHIELD_SESSION_FREE_URL                '(^/robots\.txt$)|(^/favicon\.ico$)|(^/static/)|(^/img/)|(^/error/)|(^/info)|(^/renew)|(^/en/)|(^/de/)|(^/js/)|(^/fonts/)|(^/css/)|(^/private/css/)'
MOD_MSHIELD_SESSION_HARD_TIMEOUT            18800
MOD_MSHIELD_SESSION_TIMEOUT_URL             /renew/renew.html
MOD_MSHIELD_SESSION_RENEW_URL               '^/renew'
MOD_MSHIELD_URL_AFTER_RENEW                 '/'
MOD_MSHIELD_SESSION_INACTIVITY_TIMEOUT      14400
MOD_MSHIELD_ALL_SHM_SPACE_USED_URL          /error/session_shm_used.html
MOD_MSHIELD_SESSION_DESTROY                 '^/private/logout/'
MOD_MSHIELD_SESSION_DESTROY_URL             /logout/index.html
MOD_MSHIELD_AUTHORIZATION_ENABLED           On
MOD_MSHIELD_GLOBAL_LOGON_SERVER_URL         /login/login.html
MOD_MSHIELD_GLOBAL_LOGON_AUTH_COOKIE_NAME   LOGON
MOD_MSHIELD_GLOBAL_LOGON_AUTH_COOKIE_VALUE  ok
MOD_MSHIELD_AUTHORIZED_LOGON_URL            '^/login/'
MOD_MSHIELD_ENABLED_RETURN_TO_ORIG_URL      Off
MOD_MSHIELD_USERNAME_VALUE                  MOD_MSHIELD_USERNAME

MOD_MSHIELD_FRAUD_DETECTION_ENABLED         On
MOD_MSHIELD_FRAUD_LEARNING_MODE             Off
MOD_MSHIELD_FRAUD_VALIDATION_THRESHOLD      3
MOD_MSHIELD_FRAUD_DETECTED_URL              /error/fraud_detected.html
MOD_MSHIELD_FRAUD_ERROR_URL                 /error/fraud_error.html
MOD_MSHIELD_KAFKA_BROKER                    127.0.0.1:9092
MOD_MSHIELD_KAFKA_TOPIC_ANALYSE             MarkovClicks
MOD_MSHIELD_KAFKA_TOPIC_USERMAPPING         MarkovLogins
MOD_MSHIELD_KAFKA_TOPIC_URL_CONFIG          MarkovUrlConfigs
MOD_MSHIELD_KAFKA_MSG_DELIVERY_TIMEOUT      3
MOD_MSHIELD_KAFKA_DELIVERY_CHECK_INTERVAL   100000
MOD_MSHIELD_REDIS_SERVER                    127.0.0.1
MOD_MSHIELD_REDIS_PORT                      6379
MOD_MSHIELD_REDIS_RESPONSE_TIMEOUT          3
MOD_MSHIELD_REDIS_CONNECTION_TIMEOUT        3

# Place your URL ratings in the following config file:
Include conf/extra/mod_mshield_url_rating.conf

<Location /private>
    MOD_MSHIELD_LOGON_REQUIRED      On
    ProxyPass                       http://localhost:8888/
</Location>

<Location /private/chat>
    MOD_MSHIELD_LOGON_REQUIRED  On
    MOD_MSHIELD_LOGON_SERVER_URL    /login/login2.html
    ProxyPass   http://localhost:8888/
</Location>

<Location /private/request-header>
    MOD_MSHIELD_LOGON_REQUIRED  On
    MOD_MSHIELD_LOGON_SERVER_URL    /login/login1.html
    ProxyPass   http://localhost:8888/
</Location>
```

## Configuration value explanations
Check [SETTINGS](SETTINGS.md) to get an explanation of all possible configuration values.

## URL configurations
For the URL criticality level definitions, create another file `conf/extra/mod_mshield_url_rating.conf`. Use the following format:
```bash
#################################################################################
#   MOD_MSHIELD url ratings
#################################################################################

MOD_MSHIELD_URL "^/public*" 0
MOD_MSHIELD_URL "^/private*" 4
MOD_MSHIELD_URL "^/transaction*" 5
```
**Hint:** Possible criticality levels are from 0 to 5 where 0 means "not critical at all" and 5 means "super critical".

Finally restart apache httpd in order to apply the new module and its configuration:
```bash
/etc/init.d/apache_but restart
```