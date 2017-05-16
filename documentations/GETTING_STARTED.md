# Getting started

## Content
* [Prerequisites](#markdown-header-prerequisites)
* [First hand on](#markdown-header-first-hands-on)
* [Debug](#markdown-header-first-debug)
* [Clean up](#markdown-header-first-clean-up)

## Prerequisites
*  You need the `mod_mshield` repository checked out on our system.
*  The only package requirement is, that `docker` and `docker-compose` is installed.
*  Only macOS or Linux is currently supported.

## First hands on
In order to start the demo application, hit `make demo` inside the `mod_mshield` repository directory and wait a minute until `Finished!` shows up. Now you should be able to visit [https://localhost](https://localhost) and play with the demo web application. If you visit "Pre-Auth Demo" -> "Echo Request header" you are accessing `/private` which means your session will be rated against the models (because `/private` is configured as critical area inside the web app).

![Echo_Request_Header](https://bitbucket.org/markovshield/mod_mshield/raw/develop/resources/Echo_Request_Header.png)

**Note**: In the demo web application there are now real trained models because this needs historical session data which is not available in the demo web application.

## Debug
If you run into problems, check the following steps:

1.  Make sure all containers are started:
      - `mshielddemo_schema_registry_1`
      - `mshielddemo_taskmanager_1`
      - `mshielddemo_broker_1`
      - `jobmanager`
      - `mshielddemo_zookeeper_1`
      - `mshielddemo_mshield_backend_1`
      - `mshielddemo_mshield_reverse_proxy_1`
      - `mshielddemo_redis_1`

    If one of them is missing, run `make shutdown-demo` and try again to start the environment using `make demo`.

2.  Make sure the following 2 processes are running:
      - `flink run -c ch.hsr.markovshield.flink.MarkovShieldAnalyser --jobmanager jobmanager:6123 flink/target/flink-1.0-SNAPSHOT-jar-with-dependencies.jar`
      - `java -cp kafka-stream/target/kafka-stream-1.0-SNAPSHOT-jar-with-dependencies.jar ch.hsr.markovshield.kafkastream.MarkovShieldClickstreams`

TODO: Pack these 2 processes each into a container and start them with the docker-compose file.

## Clean up
To clean up the whole markovshield demo environment, run `make shutdown-demo`.
