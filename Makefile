#############################################
#
# mod_but Makefile V1.0 by BUT
#
#############################################
# $Id: Makefile 160 2011-02-01 16:45:08Z rblum $

#APXS=/usr/bin/apxs2
APXS=/opt/applic/httpd/bin/apxs

# APXSFLAGS=-c -i -a -Wc,-O2 -Wc,-Wall -I /zpool/applic/pcre/pcre-8.0/
# APXSFLAGS=-c -i -a -Wc,-O2 -Wc,-Wall -I /root/httpd-2.2.14/srclib/pcre/
# CentOS APXSFLAGS=-c -i -a -Wc,-O2 -Wc,-Wall -I /usr/include
# APXSFLAGS=-c -i -a -Wc,-O2 -Wc,-Wall,-DMOD_BUT_SESSION_COUNT=10 -Wc,-DMOD_BUT_COOKIESTORE_COUNT=30
APXSFLAGS=-c -i -a -Wc,-O0 -Wc ,-Wall -Wc, -DMOD_BUT_SESSION_COUNT=10 -Wc, -DMOD_BUT_COOKIESTORE_COUNT=30 -Wc


SRC= \
	mod_but.c \
	mod_but_regexp.c \
	mod_but_redirect.c \
	mod_but_cookie.c \
	mod_but_access_control.c \
	mod_but_request_filter.c \
	mod_but_response_filter.c \
	mod_but_config.c \
	mod_but_session.c \
	mod_but_shm.c

all: mod_but

mod_but: $(SRC)
	$(APXS) $(APXSFLAGS) $(SRC)

clean:
	rm -rf *.la *.slo *.o *.lo .libs

.PHONY: all mod_but clean
