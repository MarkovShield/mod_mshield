#############################################
#
# mod_mshield Makefile V1.0 by MSHIELD
#
#############################################

# Change the pcre path to your real pcre path:
PCRE = /opt/applic/pcre-8.39/include

APXSFLAGS = -I$(PCRE) -v -c
APXSFLAGSEND = -Wc,-O0 -Wc,-Wall -Wc,-DMOD_MSHIELD_SESSION_COUNT=100000 -Wc,-DMOD_MSHIELD_COOKIESTORE_COUNT=300000 -Wc,-g -Wc,-Wno-unused-function

UNAME = $(shell uname)

LIBS = -lrdkafka -lz -lpthread -lm -levent -lhiredis

ifeq ($(UNAME), Linux)
LIBS += -lrt
endif

SRC = 								\
	mod_mshield.c 					\
	mod_mshield_regexp.c 			\
	mod_mshield_redirect.c 			\
	mod_mshield_cookie.c 			\
	mod_mshield_access_control.c 	\
	mod_mshield_request_filter.c 	\
	mod_mshield_response_filter.c 	\
	mod_mshield_config.c 			\
	mod_mshield_session.c 			\
	mod_mshield_shm.c 				\
	mod_mshield_kafka.c 			\
	mod_mshield_redis.c 			\
	cJSON.c

dev: APXSCMD = apxs
dev: APXSFLAGS += $(APXSFLAGSEND)

deploy: APXSCMD = /opt/applic/httpd/bin/apxs
deploy: APXSFLAGS += -a -i $(APXSFLAGSEND)

docker-compile: APXSCMD = apxs
docker-compile: APXSFLAGS += -a -i $(APXSFLAGSEND)

compile:
	docker run --rm 						\
	-v `pwd`:/opt 							\
	pschmid/apache_module_compiler 			\
	/bin/bash -c 'make docker-compile && cp /usr/local/apache2/modules/mod_mshield.so /opt && make clean'

compile-librdkafka:
	docker run --rm 						\
	-v `pwd`:/opt 							\
	pschmid/librdkafka_compiler 			\
	/bin/bash -c 'cp /tmp/librdkafka/src/librdkafka.so.1 /opt/'

demo: compile compile-librdkafka shutdown-demo
	cp librdkafka.so.1 examplesite/reverseproxy
	cp mod_mshield.so examplesite/reverseproxy
	docker-compose -p mshield-demo -f examplesite/docker-compose.yml up --build -d
	@echo Finished! Please give MarkovShield a few seconds to start everything in the background. Visit https://localhost to try markovshield.

shutdown-demo:
	docker-compose -p mshield-demo -f examplesite/docker-compose.yml down
	rm -rf examplesite/zk-txt-logs
	rm -rf examplesite/kafka-data
	rm -rf examplesite/zk-data
	rm -rf examplesite/state-store

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

.PHONY: mod_mshield dev deploy docker-compile compile compile-librdkafka demo shutdown-demo all docs clean-docs clean
