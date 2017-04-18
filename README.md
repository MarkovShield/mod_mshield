# mod_mshield

## Installation

### Prerequisites
On Linux (Debian based):
```bash
apt-get install librdkafka1
```
On macOS:
```bash
brew insall librdkafka
```

### Installation itself
```bash
cd /opt/source/
git clone git@bitbucket.org:markovshield/mod_mshield.git
cd mod_mshield && git checkout develop
make -f Makefile.deployment
```

## Configuration

In `httpd.conf` load the module:
```bash
LoadModule mshield_module modules/mod_mshield.so

# Place your mod_mshield configuration directives in the following file:
Include conf/extra/httpd-preauth.conf
```
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
MOD_MSHIELD_SESSION_FREE_URL                '(^/robots\.txt$)|(^/favicon\.ico$)|(^/static/)|(^/img/)|(^/error/)|(^/info)|(^/renew)|(^/en/)|(^/de/)|(^/js/)|(^/fonts/)|(^/css/)'
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
MOD_MSHIELD_KAFKA_BROKER                    127.0.0.1:9092
MOD_MSHIELD_KAFKA_GROUP_ID                  mshield
MOD_MSHIELD_KAFKA_RESULT_QUERY_INTERVAL     10
MOD_MSHIELD_KAFKA_RESULT_TIMEOUT            3000
MOD_MSHIELD_KAFKA_TOPIC_ANALYSE             mshield-analyse
MOD_MSHIELD_KAFKA_TOPIC_ANALYSE_RESULT      mshield-analyse-result
MOD_MSHIELD_KAFKA_TOPIC_USERMAPPING         mshield-user-mapping
MOD_MSHIELD_KAFKA_TOPIC_URL_CONFIG          mshield-url-config

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
For the URL criticality level definitions, create another file `conf/extra/mod_mshield_url_rating.conf`. Use the following format (`0` = non-critical, `1` = critical) and make sure to define the URLs **without** trailing slash::
```bash
#################################################################################
#   MOD_MSHIELD url ratings
#################################################################################

MOD_MSHIELD_URL /public 0
MOD_MSHIELD_URL /login/login.php 1
MOD_MSHIELD_URL /private 1
MOD_MSHIELD_URL /private/1 1
MOD_MSHIELD_URL /private/1/printheader.php 1
```

Finally restart apache httpd in order to apply the new module and its configuration:
```bash
/etc/init.d/apache_but restart
```

## Debugging

### Apache logs
```bash
tail -f /opt/applic/httpd/logs/error_log
```

### Test kafka connection
Listen on topic (e.g. `mshield-analyse`):
```bash
kafkacat -C -b 192.168.56.50 -t mshield-analyse
```
Post something to the topic (e.g. `mshield-analyse`):
```bash
echo "Hallo" | kafkacat -P -b 192.168.56.50 -t mshield-analyse
```
Post request rating to `mshield-analyse-result`:
```bash
echo "KEY,Test rating = SUSPICIOUS" | kafka-console-producer.sh \
    --broker-list localhost:9092 \
    --topic mshield-analyse-result \
    --property parse.key=true \
    --property key.separator=,
kafkacat -C -b 192.168.56.50 -t mshield-analyse-result
```

## Development

### Prerequisites

#### Git
Also clone the submodule librdkafka:
```bash
git submodule update --init
```

#### Dependent packages
In order to compile this module, you will need the following packages installed:

* `apr`
* `apr-utils`
* `httpd`
* `pcre`

##### macOS
If you are using macOS, just install there dependencies via Brew:
```bash
brew install apr apr-util homebrew/apache/httpd24 pcre
```

#### httpd, apr, apr-util header files
This module needs some header file from httpd, apr and apr-utils. By default these header files are linked in the CMakeLists.txt file:
```
include_directories(
    /usr/local/Cellar/httpd24/2.4.25/include/httpd/
    /usr/local/Cellar/apr-util/1.5.4_4/libexec/include/apr-1/
    /usr/local/Cellar/apr/1.5.2_3/libexec/include/apr-1/
)
```
Change these paths to once from your OS, if you are not using macOS and Brew.

### How to compile
Recommended (**make sure `apxs` is in your path**):
```bash
make
```

Alternative:
```bash
./do_compile.sh
```

Another alternative: Do it the manual way:
```bash
cd ../mod_mshield/
apxs -c mod_mshield.c
```
**Hint:**
* Add parameter `-i` for automatic installation to the httpd module directory.
* Add parameter `-a` to enable it automatically.
* The absolute path for apxs on macOS: `/usr/local/Cellar/httpd24/2.4.25/bin/apxs`

### Test module in httpd
In the Hacking-Lab VM:
```bash
/etc/init.d/apache_but restart
```

On your development system:
```bash
cd /opt/applic/httpd/bin
sudo ./httpd -f /opt/applic/httpd/conf/httpd.conf -e debug -DFOREGROUND
```

### Kafka
#### Show consumer groups
```bash
kafka-consumer-groups.sh --list --bootstrap-server localhost:9092
```

### Cleanup compilation stuff
```bash
make clean
```













# OLD Stuff

## Dependency compilation
### librdkafka
Needs `openssl` installed. Also you need to change `LIBTOOLIZE="libtoolize"` to `LIBTOOLIZE="glibtoolize"` in the `autogen.sh` script if you are using macOS.
```bash
./autogen.sh
export CPPFLAGS="-I/usr/local/opt/openssl/include/ -I/opt/markovshield_applic/apr-1.5.2/include/apr-1/ -I/opt/markovshield_applic/apr-util-1.5.4/include/apr-1/"
./configure
```
