# Development

## Content
* [Prerequisites](#prerequisites)
    * [Dependent packages](#dependent-packages)
        * [macOS](#macos)
    * [httpd, apr, apr-util header files](#httpd-apr-apr-util-header-files)
* [How to compile](#how-to-compile)
* [Test the module](#test-the-module)
* [Cleanup compilation stuff](#cleanup-compilation-stuff)
* [Debugging](#debugging)
* [Publish images](#publish-images)

## Prerequisites

### Dependent packages
In order to compile this module, you will need the sources of the following packages:
* `apr`
* `apr-utils`
* `httpd`
* `pcre`
* `librdkafka` (see [edenhill/librdkafka](https://github.com/edenhill/librdkafka))
* `hiredis` (see [redis/hiredis](https://github.com/redis/hiredis))

Depending on your OS you can also install these packages via package manager.

#### macOS
If you are using macOS, you can just install these packages via Brew:
```bash
brew install apr apr-util homebrew/apache/httpd24 pcre librdkafka hiredis
```

### httpd, apr, apr-util header files (optional)
**Note**: Only needed if you use cmake for example if your development IDE is CLion.

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
Change these paths to the ones from your OS if you are not using macOS and Brew.

## How to compile
There are basically two ways to compile mod_mshield. If you just want to compile it, choose the second way - the "no dependencies" way. If you want to develop mod_mshield, use the first one.

### Dependencies installed on your system
Compile the module without installation and without enable it automatically (**make sure `apxs` is in your path**):
```bash
make dev
```
**Hint:** The absolute path for apxs on macOS: `/usr/local/Cellar/httpd24/2.4.25/bin/apxs`

Alternatively, you can compile, install and enable the module in the httpd.conf:
```bash
make deploy
```

### No dependencies installed on your system
Its also possible to compile the module using a docker compiler image. This has the huge advantage that no depencencies have to be installed on our local compiling system (e.g. your notebook). Use:
```bash
make compile
```
**Important**: To run the module inside apache the dependencies are still required! So make sure to install the dependent packages before you enable and run mod_mshield inside your apache.

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

## Debugging
For debugging instructions/hints please have a look at [DEBUG](DEBUG.md).

## Publish images
To build the MarkovShield Docker images and publish them to a registry (e.g. Docker Hub), change the `HUBPREFIX` inside the `Makefile` to your Docker Hub username/organisation name and hit `make publish`.
