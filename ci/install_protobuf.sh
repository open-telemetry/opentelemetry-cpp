#!/bin/bash

# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

set -e

[ -z "${PROTOBUF_VERSION}" ] && export PROTOBUF_VERSION="21.12"

cd /
wget https://github.com/google/protobuf/releases/download/v${PROTOBUF_VERSION}/protobuf-cpp-3.${PROTOBUF_VERSION}.tar.gz
tar zxf protobuf-cpp-${PROTOBUF_VERSION}.tar.gz --no-same-owner
cd protobuf-${PROTOBUF_VERSION}
./configure
make -j $(nproc) && make install
ldconfig
