# Development

## Content
* [Prerequisites](#markdown-header-prerequisites)
    * [Dependent packages](#markdown-header-dependent-packages)
        * [macOS](#markdown-header-macos)
    * [httpd, apr, apr-util header files](#markdown-header-httpd-apr-apr-util-header-files)
* [How to compile](#markdown-header-how-to-compile)
* [Test the module](#markdown-header-test-the-module)
* [Cleanup compilation stuff](#markdown-header-cleanup-compilation-stuff)
* [Debugging & helpful commands](#markdown-header-debugging-helpful-commands)
    * [Apache logs](#markdown-header-apache-logs)
    * [Kafka](#markdown-header-kafka)
        * [Test kafka connection](#markdown-header-test-kafka-connection)
        * [Show consumer groups](#markdown-header-show-consumer-groups)
        * [Send test message to Kafka](#markdown-header-send-test-message-to-kafka)
    * [Redis](#markdown-header-redis)
        * [Test Redis connection](#markdown-header-test-redis-connection)

## Prerequisites

### Dependent packages
In order to compile this module, you will need the sources of the following packages:
* `apr`
* `apr-utils`
* `httpd`
* `pcre`
* `libevent` (see [nmathewson/Libevent](https://github.com/nmathewson/Libevent))
* `librdkafka` (see [edenhill/librdkafka](https://github.com/edenhill/librdkafka))
* `hiredis` (see [redis/hiredis](https://github.com/redis/hiredis))

Depending on your OS you can also install these packages via package manager.

#### macOS
If you are using macOS, you can just install these packages via Brew:
```bash
brew install apr apr-util homebrew/apache/httpd24 pcre librdkafka hiredis libevent
```

### httpd, apr, apr-util header files
You first need to create a `CMakeLists.txt` file because its inside `.gitignore`.
mod_mshield needs some header files from httpd, apr and apr-utils. By default these header files are linked in the `CMakeLists.txt` file:
```
include_directories(
    /usr/local/Cellar/httpd24/2.4.25/include/httpd/
    /usr/local/Cellar/apr-util/1.5.4_4/libexec/include/apr-1/
    /usr/local/Cellar/apr/1.5.2_3/libexec/include/apr-1/
    /usr/local/Cellar/hiredis/0.13.3/include/hiredis/
)
```
Change these paths to once from your OS if you are not using macOS and Brew.

## How to compile
Compile the module without installation and without enable it automatically (**make sure `apxs` is in your path**):
```bash
make dev
```
Alternatively, you can compile, install and enable the module in the httpd.conf:
```bash
make deploy
```
**Hint:** The absolute path for apxs on macOS: `/usr/local/Cellar/httpd24/2.4.25/bin/apxs`

## Test the module
On your development system:
```bash
cd /PATH/TO/httpd/bin
sudo ./httpd -f /PATH/TO/httpd/conf/httpd.conf -e info -DFOREGROUND
```

If you are working with the Hacking-Lab VM ([hacking-lab.com](https://media.hacking-lab.com/)), try something like the following to try out the module. Make sure to change the paths based on your environment (in `Makefile`).
```bash
mkdir -p /opt/source/
cd /opt/source/
git clone git@bitbucket.org:markovshield/mod_mshield.git
cd mod_mshield
make deploy
/etc/init.d/apache_but restart
```

## Cleanup compilation stuff
```bash
make clean
```

## Debugging & helpful commands

### Apache logs
```bash
tail -f /opt/applic/httpd/logs/*error_log
```

### Kafka

#### Test kafka connection
Listen on topic (e.g. `MarkovClicks`):
```bash
kafkacat -C -b 192.168.56.50 -t MarkovClicks
```
Post something to the topic (e.g. `MarkovClicks`):
```bash
echo "Hallo" | kafkacat -P -b 192.168.56.50 -t MarkovClicks
```

#### Show consumer groups
```bash
kafka-consumer-groups.sh --list --bootstrap-server localhost:9092
```

#### Send test message to Kafka
```bash
echo $'xtTALCofbVIMEmuJzd95Me0prdFNKt%{"sessionUUID":	"xtTALCofbVIMEmuJzd95Me0prdFNKt","clickUUID":	"zugbwerz23g8gzbhb","timeStamp":	1493639064719,"url":	"/private/request-header/","urlRiskLevel":	4,"validationRequired":	true}' | kafka-console-producer.sh \
    --broker-list localhost:9092 \
    --topic MarkovClicks \
    --property parse.key=true \
    --property key.separator=%
```

### Redis

#### Listen on multiple channels
```bash
redis-cli psubscribe WQ*
```

#### Test Redis connection
Publish engine result (e.g. `FRAUD`) to channel `zugbwerz23g8gzbhb` :
```bash
redis-cli publish zugbwerz23g8gzbhb FRAUD
```
**Hint:** The `clickUUID` is used as channel ID.
