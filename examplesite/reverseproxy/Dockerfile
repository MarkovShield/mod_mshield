FROM debian:jessie

MAINTAINER Philip Schmid

# Install some useful tools for debugging purposes, httpd and php71
RUN apt-get update && apt-get install -y \
		vim 						\
		apache2 				\
    less 						\
    net-tools 			\
		locate 					\
		wget 						\
		libhiredis0.10 	\
		libevent-dev 		\
		libcrypto++9 		\
		liblz4-dev 			\
		liblz4-1 				\
		ca-certificates

# Install php 7.0
RUN wget --no-check-certificate https://www.dotdeb.org/dotdeb.gpg -P /tmp
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
COPY examplesite/reverseproxy/apache_mshield_reverse_proxy.sh /usr/local/bin/
RUN chmod +x /usr/local/bin/apache_mshield_reverse_proxy.sh

COPY examplesite/reverseproxy/html/ /var/www/html/

COPY examplesite/reverseproxy/conf/ports.conf /etc/apache2/
COPY examplesite/reverseproxy/conf/security.conf /etc/apache2/conf-available/
COPY examplesite/reverseproxy/conf/httpd-ssl.conf /etc/apache2/sites-available/
COPY examplesite/reverseproxy/conf/httpd-preauth.conf /etc/apache2/sites-available/
COPY examplesite/reverseproxy/conf/httpd-forensic-readiness.conf /etc/apache2/conf-available/
COPY examplesite/reverseproxy/conf/httpd-hsts.conf /etc/apache2/conf-available/

RUN ln -s /etc/apache2/conf-available/security.conf /etc/apache2/conf-enabled/security.conf
RUN ln -s /etc/apache2/sites-available/httpd-ssl.conf /etc/apache2/sites-enabled/httpd-ssl.conf
RUN ln -s /etc/apache2/sites-available/httpd-preauth.conf /etc/apache2/sites-enabled/httpd-preauth.conf
RUN ln -s /etc/apache2/conf-available/httpd-forensic-readiness.conf /etc/apache2/conf-enabled/httpd-forensic-readiness.conf
RUN ln -s /etc/apache2/conf-available/httpd-hsts.conf /etc/apache2/conf-enabled/httpd-hsts.conf

RUN mkdir -p /etc/pki/certs /etc/pki/keys
COPY examplesite/reverseproxy/samplecerts/localhost.crt /etc/pki/certs/
COPY examplesite/reverseproxy/samplecerts/localhost.key /etc/pki/keys/

# Copy some library dependencies
COPY librdkafka.so.1 /usr/lib/x86_64-linux-gnu/

# Enable mod_mshield
COPY examplesite/reverseproxy/conf/mod_mshield_url_rating.conf /etc/apache2/conf-available/

COPY mod_mshield.so /usr/lib/apache2/modules/
COPY examplesite/reverseproxy/conf/mshield.load /etc/apache2/mods-available/
COPY examplesite/reverseproxy/conf/mshield.conf /etc/apache2/conf-available/

RUN a2enconf mod_mshield_url_rating mshield
RUN a2enmod mshield

# Enable required apache modules
RUN a2enmod 						\
		authn_file 					\
		authn_core 					\
		authz_host 					\
		authz_groupfile 		\
		authz_user 					\
		authz_core 					\
		access_compat 			\
		auth_basic 					\
		socache_shmcb 			\
		reqtimeout 					\
		filter 							\
		mime 								\
		log_forensic 				\
		log_debug 					\
		\env 								\
		headers 						\
		unique_id 					\
		setenvif 						\
		xml2enc 						\
		proxy 							\
		proxy_connect 			\
		proxy_ftp 					\
		proxy_http 					\
		proxy_fcgi 					\
		proxy_scgi 					\
		proxy_wstunnel 			\
		proxy_ajp 					\
		proxy_balancer 			\
		proxy_express 			\
		proxy_html 					\
		slotmem_shm 				\
		ssl 								\
		lbmethod_byrequests \
		lbmethod_bytraffic 	\
		lbmethod_bybusyness \
		status 							\
		autoindex 					\
		dir 								\
		alias 							\
		rewrite 						\
		php7.0 							\
		file_cache

# Redirect logs to stdout and stderr in order to see it with docker logs
RUN ln -sf /dev/stdout /var/log/apache2/access.log && ln -sf /dev/stderr /var/log/apache2/error.log

# Expose the backend port for linked containers
EXPOSE 80
EXPOSE 443

# Start the apache service
CMD ["/usr/local/bin/apache_mshield_reverse_proxy.sh"]
