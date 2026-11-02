#!/bin/bash

# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

set -e

BAZELISK_VERSION=v1.28.1

case "$(uname -m)" in
  x86_64) BAZELISK_ARCH=amd64 ;;
  aarch64) BAZELISK_ARCH=arm64 ;;
  *) echo "E: unsupported architecture $(uname -m)" >&2; exit 1 ;;
esac

wget -O /usr/local/bin/bazel https://github.com/bazelbuild/bazelisk/releases/download/$BAZELISK_VERSION/bazelisk-linux-$BAZELISK_ARCH
chmod +x /usr/local/bin/bazel
