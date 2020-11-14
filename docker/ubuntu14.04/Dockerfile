FROM ubuntu:14.04
ENV DEBIAN_FRONTEND noninteractive

## Update cache and upgrade image
RUN apt-get -y update && apt-get -y upgrade && apt-get -y dist-upgrade

## Build environment packages
RUN apt-get install -qq -y --ignore-missing \
	apt-utils		\
	automake		\
	build-essential		\
	bzip2			\
	cmake			\
	curl			\
	git			\
	libcurl4-openssl-dev	\
	libssl-dev		\
	make			\
	pkg-config		\
	python			\
	sudo			\
	tar			\
	zip			\
	unzip			\
	wget			\
	zlib1g-dev

# ENTRYPOINT bash
CMD /bin/bash
