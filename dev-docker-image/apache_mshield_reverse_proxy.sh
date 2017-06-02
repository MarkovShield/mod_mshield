#!/bin/bash

# Apache gets grumpy about PID files pre-existing
rm -rf /var/run/apache2/*

valgrind --trace-children=yes --trace-syscalls=yes --tool=none /usr/sbin/apachectl -DFOREGROUND -k start -e debug -X

#exec /usr/sbin/apachectl -DFOREGROUND -k start -e warn
