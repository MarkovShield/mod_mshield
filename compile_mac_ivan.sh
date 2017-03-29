#!/bin/bash
/opt/applic/httpd/bin/apxs -I /opt/applic/pcre-8.39/include/ -c -i -Wc,-O0 -Wc,-Wall -Wc,-DMOD_BUT_SESSION_COUNT=10 -Wc,-DMOD_BUT_COOKIESTORE_COUNT=30 mod_but.c

