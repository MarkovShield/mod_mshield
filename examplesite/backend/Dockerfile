FROM debian:jessie

MAINTAINER Philip Schmid

# Install some useful tools for debugging purposes, httpd and php71
RUN apt-get update && apt-get install -y \
		vim 				\
		apache2 		\
    less 				\
    net-tools 	\
		locate 			\
		wget 				\
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
COPY examplesite/backend/apache_mshield_backend.sh /usr/local/bin/
RUN chmod +x /usr/local/bin/apache_mshield_backend.sh

COPY examplesite/backend/html/ /var/www/html/

COPY examplesite/backend/conf/ports.conf /etc/apache2/
COPY examplesite/backend/conf/httpd-backend.conf /etc/apache2/sites-available/
COPY examplesite/backend/conf/security.conf /etc/apache2/conf-available/
COPY examplesite/backend/conf/proxy-html.conf /etc/apache2/conf-available/
RUN ln -s /etc/apache2/sites-available/httpd-backend.conf /etc/apache2/sites-enabled/httpd-backend.conf
RUN ln -s /etc/apache2/conf-available/security.conf /etc/apache2/conf-enabled/security.conf

# Enable required apache modules
RUN a2enmod 				\
		proxy 					\
		proxy_connect 	\
		proxy_ftp  			\
		dir 						\
		proxy_http 			\
		proxy_html 			\
		proxy_wstunnel 	\
		proxy_ajp 			\
		proxy_balancer 	\
		ssl 						\
		log_forensic 		\
		log_debug 			\
		xml2enc

# Redirect logs to stdout and stderr in order to see it with docker logs
RUN ln -sf /dev/stdout /var/log/apache2/access.log && ln -sf /dev/stderr /var/log/apache2/error.log

# Expose the backend port for linked containers
EXPOSE 8888

# Start the apache service
CMD ["/usr/local/bin/apache_mshield_backend.sh"]
