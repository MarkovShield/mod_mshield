#############################################
#
# mod_mshield Makefile V1.0 by BUT
#
#############################################
# $Id: Makefile 160 2011-02-01 16:45:08Z rblum $

#APXS=/usr/bin/apxs2
APXS=/opt/applic/httpd/bin/apxs

# APXSFLAGS=-c -i -a -Wc,-O2 -Wc,-Wall -I /zpool/applic/pcre/pcre-8.0/
# APXSFLAGS=-c -i -a -Wc,-O2 -Wc,-Wall -I /root/httpd-2.2.14/srclib/pcre/
# CentOS APXSFLAGS=-c -i -a -Wc,-O2 -Wc,-Wall -I /usr/include
# APXSFLAGS=-c -i -a -Wc,-O2 -Wc,-Wall,-DMOD_MSHIELD_SESSION_COUNT=10 -Wc,-DMOD_MSHIELD_COOKIESTORE_COUNT=30
APXSFLAGS=-I /opt/applic/pcre-8.39/include -c -i -a -Wc,-O0 -Wc ,-Wall -Wc, -DMOD_MSHIELD_SESSION_COUNT=10 -Wc, -DMOD_MSHIELD_COOKIESTORE_COUNT=30 -Wc


SRC= \
	mod_mshield.c \
	mod_mshield_regexp.c \
	mod_mshield_redirect.c \
	mod_mshield_cookie.c \
	mod_mshield_access_control.c \
	mod_mshield_request_filter.c \
	mod_mshield_response_filter.c \
	mod_mshield_config.c \
	mod_mshield_session.c \
	mod_mshield_shm.c

all: mod_mshield

mod_mshield: $(SRC)
	$(APXS) $(APXSFLAGS) $(SRC)

clean:
	rm -rf *.la *.slo *.o *.lo .libs

.PHONY: all mod_mshield clean
