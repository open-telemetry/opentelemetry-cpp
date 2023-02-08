#!/bin/bash

# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

pushd /tmp
curl -OL https://github.com/protocolbuffers/protobuf/releases/download/v21.12/protoc-21.12-linux-x86_64.zip
unzip -o protoc-21.12-linux-x86_64.zip -d /usr/local/
update-alternatives --install /usr/bin/protoc protoc /usr/local/bin/protoc 1 --force
popd
