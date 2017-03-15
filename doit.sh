make
/etc/init.d/apache_but stop
rm -rf /opt/applic/httpd/logs/*
/etc/init.d/apache_but start
