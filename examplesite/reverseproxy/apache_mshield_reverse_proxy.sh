#!/bin/bash

# Apache gets grumpy about PID files pre-existing
rm -rf /var/run/apache2/*

exec /usr/sbin/apachectl -DFOREGROUND -k start -e info
