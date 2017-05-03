#!/bin/bash
docker run --rm \
  -v /your/path/mod_mshield:/opt \
  pschmid/apache_module_compiler \
  /bin/bash -c 'make deploy && cp /usr/local/apache2/modules/mod_mshield.so /opt'
