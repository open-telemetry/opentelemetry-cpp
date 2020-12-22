FROM ubuntu:18.04
ENV DEBIAN_FRONTEND noninteractive

## Update cache and upgrade image
RUN apt-get -y update && apt-get -y upgrade && apt-get -y dist-upgrade

## Build environment packages
RUN apt-get install -qq -y --ignore-missing \
	apt-utils		\
	automake		\
	bc                      \
	build-essential		\
	bzip2			\
	cmake			\
	curl			\
	git			\
	libcurl4-openssl-dev	\
	libssl-dev		\
	libtool-bin		\
	make			\
	pkg-config		\
	protobuf-compiler	\
	libprotobuf-dev         \
	python			\
	sudo			\
	tar			\
	zip			\
	unzip			\
	wget			\
	zlib1g-dev

## Install cmake since it's an expensive operation and best be done once
RUN mkdir -p /usr/local/bin
COPY ./setup-cmake.sh /usr/local/bin/setup-cmake.sh
RUN chmod +x /usr/local/bin/setup-cmake.sh
RUN /usr/local/bin/setup-cmake.sh

# ENTRYPOINT bash
CMD /bin/bash
