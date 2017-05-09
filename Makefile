#############################################
#
# mod_mshield Makefile V1.0 by MSHIELD
#
#############################################

# Change the pcre path to your real pcre path:
PCRE = /opt/applic/pcre-8.39/include

APXSFLAGS = -I$(PCRE) -v -c
APXSFLAGSEND = -Wc,-O0 -Wc ,-Wall -Wc, -DMOD_MSHIELD_SESSION_COUNT=100 -Wc, -DMOD_MSHIELD_COOKIESTORE_COUNT=300 -Wc

UNAME = $(shell uname)

LIBS = -lrdkafka -lz -lpthread -lm -levent -lhiredis

ifeq ($(UNAME), Linux)
LIBS += -lrt
endif

SRC = \
	mod_mshield.c \
	mod_mshield_regexp.c \
	mod_mshield_redirect.c \
	mod_mshield_cookie.c \
	mod_mshield_access_control.c \
	mod_mshield_request_filter.c \
	mod_mshield_response_filter.c \
	mod_mshield_config.c \
	mod_mshield_session.c \
	mod_mshield_shm.c \
	mod_mshield_kafka.c \
	mod_mshield_redis.c \
	cJSON.c

dev: APXSCMD = apxs
dev: APXSFLAGS += $(APXSFLAGSEND)

deploy: APXSCMD = /opt/applic/httpd/bin/apxs
deploy: APXSFLAGS += -a -i $(APXSFLAGSEND)

docker-compile: APXSCMD = apxs
docker-compile: APXSFLAGS += -a -i $(APXSFLAGSEND)

all: APXSCMD = apxs
all: APXSFLAGS += $(APXSFLAGSEND)

all dev deploy docker-compile: mod_mshield

mod_mshield: $(SRC)
	$(APXSCMD) $(APXSFLAGS) $(SRC) $(LIBS)

docs:
	doxygen Doxyfile
	@echo "Documentation generated in docs"

clean-docs:
	rm -rf docs

clean:
	rm -rf *.la *.slo *.o *.lo .libs

.PHONY: mod_mshield dev deploy docker-compile all docs clean-docs clean
