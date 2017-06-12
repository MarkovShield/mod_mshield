FROM debian:jessie-backports

MAINTAINER Philip Schmid

# Install some useful tools for debugging purposes, httpd and php71
RUN apt-get update && apt-get install -y \
		vim \
    less \
    net-tools \
		locate \
		wget \
		strace \
		pstack \
		kcachegrind \
		gdb \
		valgrind \
    libapr1-dbg \
    libaprutil1-dbg \
    binutils \
    linux-tools \
		libhiredis0.10 \
		libevent-dev \
		libcrypto++9 \
		liblz4-dev \
		liblz4-1 \
		ca-certificates

# Install apache2
ENV HTTPD_PREFIX /etc/apache2
ENV PATH $HTTPD_PREFIX/bin:$PATH
RUN mkdir -p "$HTTPD_PREFIX" \
	&& chown www-data:www-data "$HTTPD_PREFIX"
WORKDIR $HTTPD_PREFIX

# library for mod_http2
ENV NGHTTP2_VERSION 1.18.1-1
ENV OPENSSL_VERSION 1.0.2k-1~bpo8+1
RUN { \
		echo 'deb http://deb.debian.org/debian stretch main'; \
	} > /etc/apt/sources.list.d/stretch.list \
	&& { \
# add a negative "Pin-Priority" so that we never ever get packages from stretch unless we explicitly request them
		echo 'Package: *'; \
		echo 'Pin: release n=stretch'; \
		echo 'Pin-Priority: -10'; \
		echo; \
# except nghttp2, which is the reason we're here
		echo 'Package: libnghttp2*'; \
		echo "Pin: version $NGHTTP2_VERSION"; \
		echo 'Pin-Priority: 990'; \
		echo; \
	} > /etc/apt/preferences.d/unstable-nghttp2

# install httpd runtime dependencies
# https://httpd.apache.org/docs/2.4/install.html#requirements
RUN apt-get update \
	&& apt-get install -y --no-install-recommends \
		libapr1 \
		libaprutil1 \
		libaprutil1-ldap \
		libapr1-dev \
		libaprutil1-dev \
		liblua5.2-0 \
		libnghttp2-14=$NGHTTP2_VERSION \
		libpcre++0 \
		libssl1.0.0=$OPENSSL_VERSION \
		libxml2 \
	&& rm -r /var/lib/apt/lists/*

ENV HTTPD_VERSION 2.4.25
ENV HTTPD_SHA1 bd6d138c31c109297da2346c6e7b93b9283993d2

# https://issues.apache.org/jira/browse/INFRA-8753?focusedCommentId=14735394#comment-14735394
ENV HTTPD_BZ2_URL https://www.apache.org/dyn/closer.cgi?action=download&filename=httpd/httpd-$HTTPD_VERSION.tar.bz2
# not all the mirrors actually carry the .asc files :'(
ENV HTTPD_ASC_URL https://www.apache.org/dist/httpd/httpd-$HTTPD_VERSION.tar.bz2.asc

# see https://httpd.apache.org/docs/2.4/install.html#requirements
RUN set -x \
	# mod_http2 mod_lua mod_proxy_html mod_xml2enc
	# https://anonscm.debian.org/cgit/pkg-apache/apache2.git/tree/debian/control?id=adb6f181257af28ee67af15fc49d2699a0080d4c
	&& buildDeps=" \
		bzip2 \
		ca-certificates \
		dpkg-dev \
		gcc \
		liblua5.2-dev \
		libnghttp2-dev=$NGHTTP2_VERSION \
		libpcre++-dev \
		libssl-dev=$OPENSSL_VERSION \
		libxml2-dev \
		zlib1g-dev \
		make \
		wget \
	" \
	&& apt-get update \
	&& apt-get install -y --no-install-recommends -V $buildDeps \
	&& rm -r /var/lib/apt/lists/* \
	\
	&& wget -O httpd.tar.bz2 "$HTTPD_BZ2_URL" \
	&& echo "$HTTPD_SHA1 *httpd.tar.bz2" | sha1sum -c - \
# see https://httpd.apache.org/download.cgi#verify
	&& wget -O httpd.tar.bz2.asc "$HTTPD_ASC_URL" \
	&& export GNUPGHOME="$(mktemp -d)" \
	&& gpg --keyserver ha.pool.sks-keyservers.net --recv-keys A93D62ECC3C8EA12DB220EC934EA76E6791485A8 \
	&& gpg --batch --verify httpd.tar.bz2.asc httpd.tar.bz2 \
	&& rm -r "$GNUPGHOME" httpd.tar.bz2.asc \
	\
	&& mkdir -p src \
	&& tar -xf httpd.tar.bz2 -C src --strip-components=1 \
	&& rm httpd.tar.bz2 \
	&& cd src \
	\
	&& gnuArch="$(dpkg-architecture --query DEB_BUILD_GNU_TYPE)" \
	&& CFLAGS="-g -pg" ./configure \
		--build="$gnuArch" \
		--prefix="$HTTPD_PREFIX" \
		--enable-mods-shared=reallyall \
	&& CFLAGS="-g -pg" make -j "$(nproc)" \
	&& make install \
	\
	&& cd .. \
	&& rm -r src man manual

# Install php 7.0
RUN wget https://www.dotdeb.org/dotdeb.gpg -P /tmp --no-check-certificate
RUN apt-key add /tmp/dotdeb.gpg
RUN echo 'deb http://packages.dotdeb.org jessie all' >> /etc/apt/sources.list
RUN echo 'deb-src http://packages.dotdeb.org jessie all' >> /etc/apt/sources.list
RUN apt-get update && apt-get install -y \
		php7.0 \
		libapache2-mod-php7.0 \
		&& rm -rf /var/lib/apt/lists/*

# Remove default httpd stuff
RUN rm -f /etc/apache2/sites-enabled/000-default.conf
RUN rm -f /var/www/html/index.html
RUN rm -f /etc/apache2/ports.conf
RUN rm -f /etc/apache2/conf-enabled/security.conf
RUN rm -f /etc/apache2/conf-available/security.conf

# Copy the required files, scripts and configurations to image
COPY apache_mshield_reverse_proxy_debug.sh /usr/local/bin/
RUN chmod +x /usr/local/bin/apache_mshield_reverse_proxy_debug.sh

COPY html/ /var/www/html/

COPY conf/ports.conf /etc/apache2/
COPY conf/security.conf /etc/apache2/conf-available/
COPY conf/httpd-ssl.conf /etc/apache2/sites-available/
COPY conf/httpd-preauth.conf /etc/apache2/sites-available/
COPY conf/httpd-forensic-readiness.conf /etc/apache2/conf-available/
COPY conf/httpd-hsts.conf /etc/apache2/conf-available/

RUN ln -s /etc/apache2/conf-available/security.conf /etc/apache2/conf-enabled/security.conf
RUN ln -s /etc/apache2/sites-available/httpd-ssl.conf /etc/apache2/sites-enabled/httpd-ssl.conf
RUN ln -s /etc/apache2/sites-available/httpd-preauth.conf /etc/apache2/sites-enabled/httpd-preauth.conf
RUN ln -s /etc/apache2/conf-available/httpd-forensic-readiness.conf /etc/apache2/conf-enabled/httpd-forensic-readiness.conf
RUN ln -s /etc/apache2/conf-available/httpd-hsts.conf /etc/apache2/conf-enabled/httpd-hsts.conf

RUN mkdir -p /etc/pki/certs /etc/pki/keys
COPY samplecerts/localhost.crt /etc/pki/certs/
COPY samplecerts/localhost.key /etc/pki/keys/

# Copy some library dependencies
COPY librdkafka.so.1 /usr/lib/x86_64-linux-gnu/

# Enable mod_mshield
COPY conf/mod_mshield_url_rating.conf /etc/apache2/conf-available/

COPY mod_mshield.so /usr/lib/apache2/modules/
COPY conf/mshield.load /etc/apache2/mods-available/
COPY conf/mshield.conf /etc/apache2/conf-available/

RUN a2enconf mod_mshield_url_rating mshield
RUN a2enmod mshield

# Enable required apache modules
RUN a2enmod \
		authn_file \
		authn_core \
		authz_host \
		authz_groupfile \
		authz_user \
		authz_core \
		access_compat \
		auth_basic \
		socache_shmcb \
		reqtimeout \
		filter \
		mime \
		log_forensic \
		log_debug \
		\env \
		headers \
		unique_id \
		setenvif \
		xml2enc \
		proxy \
		proxy_connect \
		proxy_ftp \
		proxy_http \
		proxy_fcgi \
		proxy_scgi \
		proxy_wstunnel \
		proxy_ajp \
		proxy_balancer \
		proxy_express \
		proxy_html \
		slotmem_shm \
		ssl \
		lbmethod_byrequests \
		lbmethod_bytraffic \
		lbmethod_bybusyness \
		status \
		autoindex \
		dir \
		alias \
		rewrite \
		php7.0 \
		file_cache

# Redirect logs to stdout and stderr in order to see it with docker logs
# RUN ln -sf /dev/stdout /var/log/apache2/access.log && ln -sf /dev/stderr /var/log/apache2/error.log

# Expose the backend port for linked containers
EXPOSE 80
EXPOSE 443

# Cleanup
RUN apt-get purge -y --auto-remove $buildDeps

# Some development requirements
# RUN echo 0 > /proc/sys/kernel/yama/ptrace_scope


# Start the apache service
CMD ["/usr/local/bin/apache_mshield_reverse_proxy_debug.sh"]
