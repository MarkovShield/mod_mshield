#!/bin/bash
docker run --rm \
  -v $1:/opt \
  pschmid/apache_module_compiler \
  /bin/bash -c 'make docker-compile && cp /usr/local/apache2/modules/mod_mshield.so /opt && make clean'
