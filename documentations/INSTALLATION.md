# Installation

## Content
* [The manual way](#markdown-header-the-manual-way)
    * [Prerequisites](#markdown-header-prerequisites)
    * [Important note](#markdown-header-important-note)
    * [Installation itself](#markdown-header-installation-itself)

## The manual way

### Prerequisites
On Linux (Debian based):
```bash
apt-get install librdkafka1 libhiredis-dev libhiredis0.13 apache2 apache2-dev libapr1 libapr1-dev libpcre3-dev
```

On macOS:
```bash
brew install apr apr-util homebrew/apache/httpd24 pcre librdkafka hiredis
```

### Important note
Because of the architectural design of mod_mshield, its required to use Apache 2.4+ with event-based mpm! If you try to use prefork mpm the latency of the responses will rise extremely fast - even with only about 10+ concurrent users.

To check which mpm your are running use `httpd -V`. The output should look something like this:
```bash
[root@host ~]# httpd -V
Server version: Apache/2.4.6 (CentOS)
Server built:   Apr 12 2017 21:03:28
Servers Module Magic Number: 20120211:24
Server loaded:  APR 1.4.8, APR-UTIL 1.5.2
Compiled using: APR 1.4.8, APR-UTIL 1.5.2
Architecture:   64-bit
Server MPM:     event
  threaded:     yes (fixed thread count)
    forked:     yes (variable process count)
Server compiled with....
 -D APR_HAS_SENDFILE
 -D APR_HAS_MMAP
 -D APR_HAVE_IPV6 (IPv4-mapped addresses enabled)
 -D APR_USE_SYSVSEM_SERIALIZE
 -D APR_USE_PTHREAD_SERIALIZE
 -D SINGLE_LISTEN_UNSERIALIZED_ACCEPT
 -D APR_HAS_OTHER_CHILD
 -D AP_HAVE_RELIABLE_PIPED_LOGS
 -D DYNAMIC_MODULE_LIMIT=256
 -D HTTPD_ROOT="/etc/httpd"
 -D SUEXEC_BIN="/usr/sbin/suexec"
 -D DEFAULT_PIDLOG="/run/httpd/httpd.pid"
 -D DEFAULT_SCOREBOARD="logs/apache_runtime_status"
 -D DEFAULT_ERRORLOG="logs/error_log"
 -D AP_TYPES_CONFIG_FILE="conf/mime.types"
 -D SERVER_CONFIG_FILE="conf/httpd.conf"
```

If it shows `Server MPM: event` you are good to go and ready to install mod_mshield. If it shows `Server MPM: prefork` you need to change to event-based mpm which can be done in Apache 2.4+ via dynamic shared object import (DSO). Just change `LoadModule mpm_prefork_module modules/mod_mpm_prefork.so` to `LoadModule mpm_event_module modules/mod_mpm_event.so` inside `/etc/httpd/conf.modules.d/00-mpm.conf` (or where ever your module configurations are stored). Don't forget to restart the httpd service!

For further information about Apache's Multi-Processing Modules see [http://httpd.apache.org/docs/2.4/en/mpm.html](http://httpd.apache.org/docs/2.4/en/mpm.html).

### Installation
Clone the repo:
```bash
cd /opt/source/
git clone git@bitbucket.org:markovshield/mod_mshield.git
```

Compile the module:
```bash
cd mod_mshield
make deploy
```
**Note:** Depending on your environment you perhaps need to change the `APXSCMD` variable and `PCRE` path inside the `Makefile`.
