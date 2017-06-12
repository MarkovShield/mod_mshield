# Getting started
This guide will show you how to set up a MarkovShield demo setup if you want to change some reverse proxy contents. If you just want to run MarkovShield as fast as you can and without editing the demo site, have a look at the [https://github.com/MarkovShield/install](https://github.com/MarkovShield/install) repository.

## Content
* [Prerequisites](#prerequisites)
* [First hand on](#first-hands-on)
* [Debug](#first-debug)
* [Clean up](#first-clean-up)

## Prerequisites
*  You need the `mod_mshield` repository checked out on our system.
*  The only package requirement is, that `docker` and `docker-compose` is installed.
*  Only macOS or Linux is currently supported.

## First hands on
In order to start the demo application, hit `make demo` inside the `mod_mshield` repository directory and wait a minute until `Finished!` shows up. Now you should be able to visit [https://localhost](https://localhost) and play with the demo web application. If you visit "Pre-Auth Demo" -> "Echo Request header" you are accessing `/private` which means your session will be rated against the models (because `/private` is configured as critical area inside the web app).

![Echo_Request_Header](/resources/Echo_Request_Header.png)

**Note**: In the demo web application there are now real trained user models because this would need historical session data which is not available in the demo web application.

## Debug
If you run into problems, check the following steps:

1.  Make sure all containers are started:
      - `mshielddemo_taskmanager_1`
      - `mshielddemo_broker_1`
      - `mshielddemo_jobmanager_1`
      - `mshielddemo_zookeeper_1`
      - `mshielddemo_mshield_backend_1`
      - `mshielddemo_mshield_reverse_proxy_1`
      - `mshielddemo_redis_1`
      - `mshielddemo_mshield_flink_modelupdater_1`
      - `mshielddemo_mshield_flink_analyser_1`
      - `mshielddemo_mshield_kafka_clickstreams_1`

    If one of them is missing, run `make shutdown-demo` and try again to start the environment using `make demo`.

## Clean up
To clean up the whole markovshield demo environment, run `make shutdown-demo`.
