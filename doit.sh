#!/bin/bash
make
/etc/init.d/apache_mshield stop
#rm -rf /opt/applic/httpd/logs/*
/etc/init.d/apache_mshield start
