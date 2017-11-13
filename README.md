# mod_mshield - Apache HTTPD reverse proxy module

[![Build Status](https://travis-ci.org/MarkovShield/mod_mshield.svg?branch=develop)](https://travis-ci.org/MarkovShield/mod_mshield)

## Content
* [Documentation](#documentation)
* [Further informations](#further-informations)
* [Licence](#licence)
* [Credit](#credit)

## Overview
![MarkovShield](/resources/Abstract_Architecture_Design_v6.png)

## Features
* Preauthentication of users at reverse proxy level
  * Authentication level step up enforcement based on configurable URLs risk levels
* Fraud detection & prevention using Machine Learning algorithms (MarkovShield Engine required)
  * Rate the current session based on historical user behaviour data
  * Enforce authentication in case the Engine rated the current session as suspicious
  * Drop user session if Engine has detected a fraud (e.g. man-in-the-browser trojan like Gozi)

## Documentation
In order to generate the documentation, use `make docs`. The documentation is then available at docs/index.html

Use `make clean-docs` if you want to clean the previously generated docs.

## Further informations
* [Installation](documentations/INSTALLATION.md)
* [Configuration](documentations/CONFIGURATION.md)
* [Settings](documentations/SETTINGS.md)
* [Development](documentations/DEVELOPMENT.md)
* [Debug](documentations/DEBUG.md)
* [Getting started](documentations/GETTING_STARTED.md)

## Licence
See [License](LICENSE.md) for our code.

## Credit
Thanks to:

* [kjdev/apache-mod-kafka](https://github.com/kjdev/apache-mod-kafka) for the Kafka producer part on with our implementation is based
* [edenhill/librdkafka](https://github.com/edenhill/librdkafka) for the Kafka C/C++ library
* [DaveGamble/cJSON](https://github.com/DaveGamble/cJSON) for the cJSON parser
