FROM ubuntu:14.04
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
	make			\
	pkg-config		\
	python			\
	sudo			\
	tar			\
	zip			\
	unzip			\
	wget			\
	zlib1g-dev

RUN mkdir -p /usr/local/bin

## Install cmake since it's an expensive operation and best be done once
COPY ./setup-cmake.sh /usr/local/bin/setup-cmake.sh
RUN chmod +x /usr/local/bin/setup-cmake.sh
RUN /usr/local/bin/setup-cmake.sh

## Install protobuf3 since Ubuntu 14.04 does not have protobuf3
COPY ./setup-protobuf.sh /usr/local/bin/setup-protobuf.sh
RUN chmod +x /usr/local/bin/setup-protobuf.sh
RUN /usr/local/bin/setup-protobuf.sh

# ENTRYPOINT bash
CMD /bin/bash
