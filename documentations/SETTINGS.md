# Settings

The following table shows and explains all possible mod_mshield configurations options (apache httpd directives).
A working example configuration can be found in the [CONFIGURATION](CONFIGURATION.md) documentation.

## General mod_mshield configuration directives
Directive | Required | Possible values | Default value | Description
--|--|--|--|--
MOD_MSHIELD_ENABLED | X | On, Off | On | Enable or disable mod_mshield per VirtualHost.
MOD_MSHIELD_ENABLED_RETURN_TO_ORIG_URL | | On, Off | Off | Enable or disable return to orig url.
MOD_MSHIELD_CLIENT_REFUSES_COOKIES_URL | X | Relative URL | None | Configure error URL, if browser denies Set-Cookie headers.
MOD_MSHIELD_COOKIE_NAME | | Name | MOD_MSHIELD | Name of the cookie sent to the client
MOD_MSHIELD_COOKIE_DOMAIN | | Domain | None | The domain which should be set inside the cookie.
MOD_MSHIELD_COOKIE_PATH | | Path | / | Set the path inside the cookie.
MOD_MSHIELD_COOKIE_EXPIRATION | | Date | None | Set the cookie expiration date (UTC/GMT format).
MOD_MSHIELD_COOKIE_SECURE | | On, Off | On | Set the "secure" flag on the cookie which we hand over to the user
MOD_MSHIELD_COOKIE_HTTPONLY | | On, Off | On | Set the "httponly" flag on the cookie which we hand over to the user
MOD_MSHIELD_SESSION_FREE_URL | X | Pattern | None | RegEx which matches free URLs. "Free URLs" are URLs on which mod_mshield shouldn't enforce user authentication. Usually this is meant for static content like "/js", "/images" and so on.
MOD_MSHIELD_SESSION_HARD_TIMEOUT | | 0 ... n | 3600 | Configure max session time of mod_mshield (elapsed time).
MOD_MSHIELD_SESSION_INACTIVITY_TIMEOUT | | 0 ... n | 900 | Configure inactivity timeout. This timeout must be below the session hard timeout.
MOD_MSHIELD_SESSION_TIMEOUT_URL | X | Relative URL | None | Where to redirect the client if the session timed out (hard or inactivity timeout).
MOD_MSHIELD_SESSION_RENEW_URL | | Pattern | '^/renew' | RegEx for which mod_mshield will create a new session, independent of what the client sent previously.
MOD_MSHIELD_SESSION_DESTROY | | Pattern | '^/logout/' | Logout pattern (session destroy). If a request calls this URL, it's session gets destroyed.
MOD_MSHIELD_SESSION_DESTROY_URL | X | Relative URL | None | Error URL in case the client has logged out.
MOD_MSHIELD_ALL_SHM_SPACE_USED_URL | X | Relative URL | None | Error URL in case mod_mshield is not able to store a session to the shared memory segment.
MOD_MSHIELD_AUTHORIZATION_ENABLED | X | On, Off | Off | If set to “On”, mod_mshield will test the mod_mshield session for authentication. Only authenticated users are allowed requesting protected URL’s. If set to “Off”, mod_mshield will not enforce authentication to any URL and will also disable the authentication on location/directory level (even when MOD_MSHIELD_LOGON_REQUIRED is set to "On").
MOD_MSHIELD_GLOBAL_LOGON_SERVER_URL | X | URL to login server 0 | None | URL for global logon server (default).
MOD_MSHIELD_GLOBAL_LOGON_SERVER_URL_1 | X | URL to login server 1 | None | URL for global logon server (username & password).
MOD_MSHIELD_GLOBAL_LOGON_SERVER_URL_2 | X | URL to login server 2 | None | URL for global logon server (strong authentication).
MOD_MSHIELD_GLOBAL_LOGON_AUTH_COOKIE_NAME | | Name | LOGON | Define cookie name of mod_mshield, which changes the authentication status. This cookie is sent from a backend-system as response header
MOD_MSHIELD_GLOBAL_LOGON_AUTH_COOKIE_VALUE | | Value | ok | Define cookie value of mod_mshield, which changes the authentication status.
MOD_MSHIELD_SESSION_STORE_FREE_COOKIES | | Pattern | None | Configure cookie names, which are not handled by MOD_BUT. They pass the reverse proxy without being kept within the shared memory session store
MOD_MSHIELD_SERVICE_LIST_ENABLED | | On, Off | Off | Turns the service list authorization on and off.
MOD_MSHIELD_SERVICE_LIST_COOKIE_NAME | | Name | MOD_MSHIELD_SERVICE_LIST | Name of the service list authorization Set-Cookie header.
MOD_MSHIELD_SERVICE_LIST_COOKIE_VALUE | | Pattern | '^/.*$' | Pattern for the URL’s the user is authorized. This pattern has a default value, which allowes “all URL’s”. If the login application sets another regular expression value, for example (`“^/webapp/asl\"`), the user is only authorized for the above URL’s.
MOD_MSHIELD_SERVICE_LIST_AUTH_ERROR_URL | | Relative URL | None | This value configures the error page URL, if the user is authenticated but unauthorized for requesting a certain URL. Required if MOD_MSHIELD_SERVICE_LIST_ENABLED is "On".
MOD_MSHIELD_AUTHORIZED_LOGON_URL | | Pattern | '^/.*$' | If configured, one can configure an URL, which is allowed to flag a session as authenticated. **Important:** Without this setting, all URL’s are allowed to manipulate the authentication state! Recommended is something like `'^/login/'` (be as specific as possible).
MOD_MSHIELD_URL_AFTER_RENEW | | Relative URL | '/' | Where to redirect if the client has renewed the session
MOD_MSHIELD_USERNAME_VALUE | | Value | MOD_MSHIELD_USERNAME | Login server LOGON cookie username value name (needs to be the same as defined in the login server).

## Fraud detection & prevention configuration directives
Directive | Required* | Possible values | Default value | Description
--|--|--|--|--
MOD_MSHIELD_FRAUD_DETECTION_ENABLED | | On, Off | Off | Enable or disable the fraud detection & prevention module part.
MOD_MSHIELD_FRAUD_LEARNING_MODE | | On, Off | Off | Enable of disable the learning mode from the fraud detection & prevention module part. If enabled request information will only be extracted to the Kafka middleware but no session rating will be done by the engine. The request itself won't be blocked by the module and will be directly forwarded to the webapplication after the request information extraction succeeded. This mode is meant to be used during the user behaviour learning phase.
MOD_MSHIELD_FRAUD_VALIDATION_THRESHOLD | | 0 ... 1000 | 3 | URL risk level rating threshold. Every URL risk level below this threshold won't be rated from the Engine. If you set it to 0 every request will be rated (not recommended). Possible range is given from MOD_MSHIELD_URL_CRITICALITY_LEVEL_MIN and MOD_MSHIELD_URL_CRITICALITY_LEVEL_MAX.
MOD_MSHIELD_FRAUD_DETECTED_URL | X | Relative URL | None | Where to redirect the client if a fraud was detected.
MOD_MSHIELD_FRAUD_ERROR_URL | X | Relative URL  | None | Where to redirect the client if a error occurred during the rating process which caused that the session couldn't be rated properly.
MOD_MSHIELD_KAFKA_BROKER | X | Hostname/IP:Port | None | Hostname / IP address and port of the Kafka broker.
MOD_MSHIELD_KAFKA_TOPIC_ANALYSE | | Topic name | MarkovClicks | Kafka topic name for the topic to which all click entries are sent.
MOD_MSHIELD_KAFKA_TOPIC_USERMAPPING | | Topic name | MarkovLogins | Kafka topic name for the topic to which the User <-> Session UUID mapping is sent.
MOD_MSHIELD_KAFKA_TOPIC_URL_CONFIG | | Topic name | MarkovUrlConfigs | Kafka topic name for the topic from which the URL configurations are received. This functionallity is corrently not fully implemented. Until the implementation of this feature, use the URL configuration via apache configuration file.
MOD_MSHIELD_KAFKA_MSG_DELIVERY_TIMEOUT | | 1 ... n | 3 | Timeout for the Kafka message delivery check (**in seconds**). Do not set this value too small because then many requests will be dropped and redirected to "MOD_MSHIELD_FRAUD_ERROR_URL". A too big value lets the user wait too long in case there is something wrong with the Kafka setup.
MOD_MSHIELD_KAFKA_DELIVERY_CHECK_INTERVAL | | 50000 (= 0.05ms) ... 1000000 (= 0.1ms) | 100000 | Time to sleep between kafka produce delivery report polls (**in nanoseconds**).
MOD_MSHIELD_REDIS_SERVER | X | Hostname/IP | None | Hostname / IP address of the Redis server.
MOD_MSHIELD_REDIS_PORT | | Port | None (0) | The port on which the Redis server is listening.
MOD_MSHIELD_REDIS_RESPONSE_TIMEOUT | | 1 ... 20 | 3 | How long to wait for request analyse result (**in seconds**). A too small value causes the request to be dropped before the session rating was received from the Engine. A too big value lets the user wait too long in case there is something wrong with the Redis setup or session rating.
MOD_MSHIELD_REDIS_CONNECTION_TIMEOUT | | 1 ... 5 | 3 | Redis connection timeout (**in seconds**).

\* "Required" means required only if MOD_MSHIELD_FRAUD_DETECTION_ENABLED is set to "On".

## Per directory/location configuration directives
Directive | Required* | Possible values | Default value | Description
--|--|--|--|--
MOD_MSHIELD_LOGON_SERVER_URL |  | URL to login server 0 | None | Logon server relative URL for this directory
MOD_MSHIELD_LOGON_REQUIRED |  | On, Off | Off | Logon requred for this directory. If set to “On” within a Location directive, mod_mshield will enforce an authenticated session. If set to “Off”, the URL within the Location is open for all and authentication is not enforced.
MOD_MSHIELD_LOCATION_ID |  | 0 ... n | 0 | Unique location ID for this directory. Groups backend-applications into cookie domains.
MOD_MSHIELD_AUTH_STRENGTH |  | 0 ... 2 | None | Authentication strength required for this directory
