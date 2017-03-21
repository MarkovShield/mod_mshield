#!/bin/bash
/usr/local/Cellar/httpd24/2.4.25/bin/apxs -c -Wc,-O0 -Wc,-Wall -Wc,-DMOD_BUT_SESSION_COUNT=10 -Wc,-DMOD_BUT_COOKIESTORE_COUNT=30 mod_but.c