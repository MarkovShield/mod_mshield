# mod_mshield
## Installation
(BUT)

TDB

## Configuration

In `httpd.conf` or any other vhost configuration file, add the following lines and configure them depending on your setup:
```bash
LoadModule mshield_module modules/mod_mshield_playground.so

MSHIELD_ENABLED                     1
MSHIELD_KAFKA_BROKER_IP             127.0.0.1
MSHIELD_KAFKA_BROKER_PORT           9092
MSHIELD_KAFKA_TOPIC_ANALYSE         mshield-analyse
MSHIELD_KAFKA_TOPIC_ANALYSE_RESULT  mshield-analyse-result
```
(shown here are the default values)

For the URL criticality level definitions, use the following format (`0` = non-critical, `1` = critical): 
```bash
MSHIELD_URL "/public_data" 0
MSHIELD_URL "profile" 1
```

## Development

### Prerequisites

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
**Hint for later usage:** The httpd path is `/usr/local/Cellar/httpd24/2.4.25`

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
```bash
cd /opt/markovshield_applic/httpd/bin
sudo ./httpd -f /opt/markovshield_applic/httpd/conf/httpd.conf -e debug -DFOREGROUND
```

### Cleanup compilation stuff
```bash
make clean
```
