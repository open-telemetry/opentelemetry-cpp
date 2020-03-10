#!/bin/bash

set -e

[ -z "${PROTOBUF_VERSION}" ] && export PROTOBUF_VERSION="3.11.4"

apt-get update 
apt-get install --no-install-recommends --no-install-suggests -y \
         curl

# Make sure you grab the latest version
cd /
curl -OL https://github.com/google/protobuf/releases/download/v${PROTOBUF_VERSION}/protobuf-cpp-${PROTOBUF_VERSION}.tar.gz
tar zxf protobuf-cpp-${PROTOBUF_VERSION}.tar.gz
cd protobuf-${PROTOBUF_VERSION}
./configure
make && make install
ldconfig
