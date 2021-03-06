# Configuration
The follwing steps will give you an example of how to set up mod_mshield. Be aware that depending on your apache installation paths and configuration file includes maybe have to be done in other ways.

## Content
* [Basic module configuration](#basic-module-configuration)
* [Basic mod_mshield configuration](#basic-mod_mshield-configuration)
* [Configuration value explanations](#configuration-value-explanations)
* [URL configurations](#url-configurations)
* [Performance tuning](#performance-tuning)

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

# Minimal required configuration
MOD_MSHIELD_ENABLED                         On
MOD_MSHIELD_CLIENT_REFUSES_COOKIES_URL      /error/refused_cookies.html
MOD_MSHIELD_SESSION_FREE_URL                '(^/error/refused_cookies\.html$)|(^/robots\.txt$)|(^/favicon\.ico$)|(^/static/)|(^/img/)|(^/error/)|(^/info)|(^/renew)|(^/en/)|(^/de/)|(^/js/)|(^/fonts/)|(^/css/)|(^/private/css/)'
MOD_MSHIELD_SESSION_TIMEOUT_URL             /renew/renew.html
MOD_MSHIELD_SESSION_DESTROY_URL             /logout/index.html
MOD_MSHIELD_URL_AFTER_RENEW                 /
MOD_MSHIELD_ALL_SHM_SPACE_USED_URL          /error/session_shm_used.html
MOD_MSHIELD_GLOBAL_LOGON_SERVER_URL         /login/login.html
MOD_MSHIELD_GLOBAL_LOGON_SERVER_URL_1       /login/login1.html
MOD_MSHIELD_GLOBAL_LOGON_SERVER_URL_2       /login/login2.html

# Additional configuration which is needed for example site docker container
MOD_MSHIELD_SESSION_DESTROY                 '^/session_logout'
MOD_MSHIELD_AUTHORIZATION_ENABLED           On
MOD_MSHIELD_COOKIE_SECURE                   Off
MOD_MSHIELD_COOKIE_HTTPONLY                 Off
MOD_MSHIELD_AUTHORIZED_LOGON_URL            '^/login/'

MOD_MSHIELD_FRAUD_DETECTION_ENABLED         On
MOD_MSHIELD_FRAUD_DETECTED_URL              /error/fraud_detected.html
MOD_MSHIELD_FRAUD_ERROR_URL                 /error/fraud_error.html
MOD_MSHIELD_KAFKA_BROKER                    127.0.0.1:9092
MOD_MSHIELD_REDIS_SERVER                    127.0.0.1
MOD_MSHIELD_REDIS_PORT                      6379

# Place your URL ratings in the following config file:
Include conf/extra/mod_mshield_url_rating.conf

# Make sure this Location directives are in the apache global scope!
<Location /private>
    MOD_MSHIELD_LOGON_REQUIRED  On
    ProxyPass   http://localhost:8888/
</Location>

<Location /private/chat>
    MOD_MSHIELD_LOGON_REQUIRED  On
    ProxyPass   http://localhost:8888/
</Location>

<Location /private/request-header>
    MOD_MSHIELD_LOGON_REQUIRED  On
    ProxyPass   http://localhost:8888/
</Location>
```

## Configuration value explanations
Check [SETTINGS](SETTINGS.md) to get an explanation of all possible configuration values.

## URL configurations
For the URL criticality level definitions, create another file `conf/extra/mod_mshield_url_rating.conf`.

**Important**
*  Possible criticality levels are from 0 to 1000 where 0 means "not critical at all" and 1000 means "super critical". In productive usage its important to use your own scale here related to the models which are used by the engine. For the demo its sufficient to use 0 to 5.
*  Ensure that **no** URL, which is reachable **without login**, has a bigger or equal risk level then `MOD_MSHIELD_FRAUD_VALIDATION_THRESHOLD`. If you do, there will be no real request rating and the engine will always return OK.
*  Be aware that setting a risk level of whole url ranges to a higher value then `MOD_MSHIELD_FRAUD_VALIDATION_THRESHOLD` can massively increase the load on you apache server. Ensure to only set single URLs to a critical risk level to reduce the apache work load.

Use the following format:
```bash
#################################################################################
#   MOD_MSHIELD url ratings
#################################################################################

MOD_MSHIELD_URL "^/public*" 0
MOD_MSHIELD_URL "^/private*" 4
MOD_MSHIELD_URL "^/transaction*" 5
```

Finally restart apache httpd in order to apply the new module and its configuration:
```bash
/etc/init.d/apache_but restart
```

## Performance tuning
Mod_mshield would work with 200 concurrent users without a doubt. If you have even more concurrent users on your web application, consider to ajust some event-based mpm settings. Please have a look at [https://httpd.apache.org/docs/current/mod/event.html](https://httpd.apache.org/docs/current/mod/event.html) to see which directives can be changed to improve the event-based mpm's performance.
