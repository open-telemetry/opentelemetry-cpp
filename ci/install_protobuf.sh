#!/bin/bash

# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

set -e

[ -z "${PROTOBUF_VERSION}" ] && export PROTOBUF_VERSION="21.12"

#
# Note
#
# protobuf uses two release number schemes,
# for example 3.21.12 and 21.12,
# and both tags corresponds to the same commit:
#
#   commit f0dc78d7e6e331b8c6bb2d5283e06aa26883ca7c (HEAD -> release-3.21.12, tag: v3.21.12, tag: v21.12)
#   Author: Protobuf Team Bot <protobuf-team-bot@google.com>
#   Date:   Mon Dec 12 16:03:12 2022 -0800
#
#       Updating version.json and repo version numbers to: 21.12
#
# tag v21.12 corresponds to the 'protoc version', or repo version
# tag v3.21.12 corresponds to the 'cpp version'
#
# protobuf-cpp-3.21.12.tar.gz:
# - is provided under releases/download/v21.12
# - is no longer provided under releases/download/v3.21.12,
#
# Use the "repo version number" (PROTOBUF_VERSION=21.12)
# when calling this script
#

export CPP_PROTOBUF_VERSION="3.${PROTOBUF_VERSION}"

cd /
wget https://github.com/google/protobuf/releases/download/v${PROTOBUF_VERSION}/protobuf-cpp-${CPP_PROTOBUF_VERSION}.tar.gz
tar zxf protobuf-cpp-${CPP_PROTOBUF_VERSION}.tar.gz --no-same-owner
cd protobuf-${CPP_PROTOBUF_VERSION}
./configure
make -j $(nproc) && make install
ldconfig
