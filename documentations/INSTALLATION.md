# Installation

## Content
* [The manual way](#markdown-header-the-manual-way)
    * [Prerequisites](#markdown-header-prerequisites)
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
