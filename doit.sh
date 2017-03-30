#!/bin/bash
sudo /opt/applic/httpd/bin/apachectl -k stop
make -f Makefile.ivan
rm -rf /opt/applic/httpd/logs/*
sudo /opt/applic/httpd/bin/apachectl -k start
