#!/bin/bash
/etc/init.d/apache_but stop
make -f Makefile.ivan
#rm -rf /opt/applic/httpd/logs/*
/etc/init.d/apache_but start
