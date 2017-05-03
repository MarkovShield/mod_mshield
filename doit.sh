#!/bin/bash
/etc/init.d/apache_but stop
make deploy
#rm -rf /opt/applic/httpd/logs/*
/etc/init.d/apache_but start
