# Settings

The following table shows and explains all possible mod_mshield configurations options (apache httpd directives).
A full example configuration can be found in the [CONFIGURATION](CONFIGURATION.md) documentation.

## General mod_mshield configuration directives
Directive | Possible values | Default value | Description
--|--|--|--
MOD_MSHIELD_ENABLED | On, Off | On | Enable or disable mod_mshield per VirtualHost.
MOD_MSHIELD_CLIENT_REFUSES_COOKIES_URL | URL to html site | None | Configure error URL, if browser denies Set-Cookie headers.
MOD_MSHIELD_COOKIE_NAME | Name | MOD_MSHIELD | Name of the cookie sent to the client
MOD_MSHIELD_COOKIE_DOMAIN | Domain | None | The domain which should be set inside the cookie.
MOD_MSHIELD_COOKIE_PATH | Path of cookie | / | Set the path inside the cookie.
MOD_MSHIELD_COOKIE_EXPIRATION | Date | None | Set the cookie expiration date.
MOD_MSHIELD_COOKIE_SECURE | On, Off | Off | Set the "secure" flag on the cookie which we hand over to the user
MOD_MSHIELD_COOKIE_HTTPONLY | On, Off | Off | Set the "httponly" flag on the cookie which we hand over to the user
MOD_MSHIELD_SESSION_FREE_URL | Pattern of renew | None | RegEx which matches free URLs. "Free URLs" are URLs on which mod_mshield shouldn't enforce user authentication. Usually this is meant for static content like "/js", "/images" and so on.
MOD_MSHIELD_SESSION_HARD_TIMEOUT | 0 ... n | 3600 | Configure max session time of mod_mshield (elapsed time).
MOD_MSHIELD_SESSION_INACTIVITY_TIMEOUT | 0 ... n | 900 | Configure inactivity timeout. This timeout must be below the session hard timeout.
MOD_MSHIELD_SESSION_TIMEOUT_URL | URL to html site | None | Where to redirect the client if the session timed out (hard or inactivity timeout).
MOD_MSHIELD_SESSION_RENEW_URL | Pattern of renew | '^/renew' | RegEx for which mod_mshield will create a new session, independent of what the client sent previously.
MOD_MSHIELD_URL_AFTER_RENEW | URL to html site | '/' | Where to redirect if the client has renewed the session
MOD_MSHIELD_ALL_SHM_SPACE_USED_URL |  |  |
MOD_MSHIELD_SESSION_DESTROY |  |  |
MOD_MSHIELD_SESSION_DESTROY_URL |  |  |
MOD_MSHIELD_AUTHORIZATION_ENABLED |  |  |
MOD_MSHIELD_GLOBAL_LOGON_SERVER_URL |  |  |
MOD_MSHIELD_GLOBAL_LOGON_SERVER_URL_1 |  |  |
MOD_MSHIELD_GLOBAL_LOGON_SERVER_URL_2 |  |  |
MOD_MSHIELD_GLOBAL_LOGON_AUTH_COOKIE_NAME |  |  |
MOD_MSHIELD_GLOBAL_LOGON_AUTH_COOKIE_VALUE |  |  |
MOD_MSHIELD_AUTHORIZED_LOGON_URL |  |  |
MOD_MSHIELD_ENABLED_RETURN_TO_ORIG_URL |  |  |
MOD_MSHIELD_USERNAME_VALUE |  |  |

## Fraud detection & prevention configuration directives
Directive | Possible values | Default value | Description
--|--|--|--
MOD_MSHIELD_FRAUD_DETECTION_ENABLED | On, Off | Off | Enable or disable the fraud detection & prevention module part.
MOD_MSHIELD_FRAUD_LEARNING_MODE | On, Off | Off | Enable of disable the learning mode from the fraud detection & prevention module part. If enabled request information will only be extracted to the Kafka middleware but no session rating will be done by the engine. The request itself won't be blocked by the module and will be directly forwarded to the webapplication after the request information extraction succeeded. This mode is meant to be used during the user behaviour learning phase.
MOD_MSHIELD_FRAUD_VALIDATION_THRESHOLD | 0 - 5 | 3 | URL risk level rating threshold. Every URL risk level below this threshold won't be rated from the Engine.
MOD_MSHIELD_FRAUD_DETECTED_URL | URL to html site | /error/fraud_detected.html | Where to redirect the client if a fraud was detected.
MOD_MSHIELD_FRAUD_ERROR_URL | URL to html site | /error/fraud_error.html | Where to redirect the client if a error occurred during the rating process which caused that the session couldn't be rated properly.
MOD_MSHIELD_KAFKA_BROKER | Hostname/IP:Port | 127.0.0.1:9092 | Hostname / IP address and port of the Kafka broker.
MOD_MSHIELD_KAFKA_TOPIC_ANALYSE | Topic name | MarkovClicks | Kafka topic name for the topic to which all click entries are sent.
MOD_MSHIELD_KAFKA_TOPIC_USERMAPPING | Topic name | MarkovLogins | Kafka topic name for the topic to which the User <-> Session UUID mapping is sent.
MOD_MSHIELD_KAFKA_TOPIC_URL_CONFIG | Topic name | MarkovUrlConfigs | Kafka topic name for the topic from which the URL configurations are received. This functionallity is corrently not fully implemented. Till the implementation of this feature, use the URL configuration via apache configuration file.
MOD_MSHIELD_KAFKA_MSG_DELIVERY_TIMEOUT | 1 ... n | 3 | Timeout for the Kafka message delivery check (**in seconds**). Do not set this value too small because then many requests will be dropped and redirected to "MOD_MSHIELD_FRAUD_ERROR_URL". A too big value lets the user wait too long in case there is something wrong with the Kafka setup.
MOD_MSHIELD_KAFKA_DELIVERY_CHECK_INTERVAL | 50000 (0.05ms) ... 1000000 | 100000 (= 0.1 ms) | Time to sleep between kafka produce delivery report polls (**in nanoseconds**).
MOD_MSHIELD_REDIS_SERVER | Hostname/IP | 127.0.0.1 | Hostname / IP address of the Redis server.
MOD_MSHIELD_REDIS_PORT | Port | 6379 | The port on which the Redis server is listening.
MOD_MSHIELD_REDIS_RESPONSE_TIMEOUT | 1 ... 20 | 3 | How long to wait for request analyse result (**in seconds**). A too small value causes the request to be dropped before the session rating was received from the Engine. A too big value lets the user wait too long in case there is something wrong with the Redis setup or session rating.
MOD_MSHIELD_REDIS_CONNECTION_TIMEOUT | 1 ... 5 | 3 | Redis connection timeout (**in seconds**).
