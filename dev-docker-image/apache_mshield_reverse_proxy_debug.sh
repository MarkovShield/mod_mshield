#!/bin/bash

# Apache gets grumpy about PID files pre-existing
rm -rf /var/run/apache2/*

# Choose between one of the following command depending on your debug scenario
#valgrind --trace-children=yes --trace-syscalls=yes --tool=callgrind /usr/sbin/apachectl -DFOREGROUND -k start -e debug -X
exec /usr/sbin/apachectl -DFOREGROUND -k start -e debug -X
