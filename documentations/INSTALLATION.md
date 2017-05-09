# Installation

## Content
* [The smooth way](#markdown-header-the-smooth-way)
    * [Prerequisites](#markdown-header-prerequisites)
    * [Installation itself](#markdown-header-installation-itself)
* [The manual way](#markdown-header-the-manual-way)
    * [Prerequisites](#markdown-header-prerequisites)
    * [Installation itself](#markdown-header-installation-itself)

## The smooth way
Using this way you won't have to install multiple requirements.

### Prerequisites
The only prerequisite is that you need `docker` installed on your system.

### Installation
Clone the repo:
```bash
cd /your/path/
git clone git@bitbucket.org:markovshield/mod_mshield.git
```

Now compile the module by executing `docker_compile.sh` with the **absolute** path to the `mod_mshield` source files.
```bash
chmod +x docker_compile.sh
./docker_compile.sh "/your/path/mod_mshield"
```

Finally place the `mod_mshield.so` module in your apache module directory and continue with the [CONFIGURATION](CONFIGURATION.md).

## The manual way

### Prerequisites
On Linux (Debian based):
```bash
apt-get install librdkafka1 libhiredis-dev libhiredis0.13 libevent-dev
```

On macOS:
```bash
brew install apr apr-util homebrew/apache/httpd24 pcre librdkafka hiredis libevent
```

### Installation
Clone the repo:
```bash
cd /opt/source/
git clone git@bitbucket.org:markovshield/mod_mshield.git
```

Compile the module:
```bash
cd mod_mshield && git checkout develop
make -f Makefile.deployment
```
**Note:** Depending on your environment you perhaps need to change the `APXS` variable and `pcre` path inside `Makefile.deployment`.
