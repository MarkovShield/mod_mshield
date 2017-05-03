![MarkovShield]()

# mod_mshield - Apache HTTPD reverse proxy module

## Content
* [Documentation](#markdown-header-documentation)
* [Further informations](#markdown-header-further-informations)
* [Licence](#markdown-header-licence)
* [Credit](#markdown-header-credit)

## Features
* Preauthentication of users at reverse proxy level
  * Authentication level step up enforcement based on configurable URLs risk levels
* Fraud detection & prevention using Machine Learning algorithms (MarkovShield Engine required)
  * Rate the current session based on historical user behaviour data
  * Enforce authentication in case the Engine reated the current session as suspicious
  * Drop user session if Engine has detected a fraud (e.g. man-in-the-browser trojan like Gozi)

## Documentation
In order to generate the documentation, use `make docs`. The documentation is then available at docs/index.html

Use `make clean-docs` if you want to clean them back again.

## Further informations
* [Installation](documentations/INSTALLATION.md)
* [Configuration](documentations/CONFIGURATION.md)
* [Development](documentations/DEVELOPMENT.md)

## Licence
See [License](LICENSE.md)) for our code.

## Credit
Thanks to:
* [kjdev/apache-mod-kafka](https://github.com/kjdev/apache-mod-kafka) for the Kafka producer part on with our implementation is based
* [edenhill/librdkafka](https://github.com/edenhill/librdkafka) for the Kafka C/C++ library
* [DaveGamble/cJSON](https://github.com/DaveGamble/cJSON) for the cJSON parser
