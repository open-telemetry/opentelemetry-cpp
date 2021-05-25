#!/bin/bash

# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

set -e

apt-get install -y clang-format-10 python3-pip git curl
pip3 install cmake_format==0.6.13
curl -L -o /usr/local/bin/buildifier https://github.com/bazelbuild/buildtools/releases/download/2.2.1/buildifier
chmod +x /usr/local/bin/buildifier
