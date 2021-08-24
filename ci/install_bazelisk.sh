#!/bin/bash

# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

set -e

BAZELISK_VERSION=v1.10.1

wget -O /usr/local/bin/bazel https://github.com/bazelbuild/bazelisk/releases/download/$BAZELISK_VERSION/bazelisk-linux-amd64
chmod +x /usr/local/bin/bazel
