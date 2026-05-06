#!/usr/bin/env bash

# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

# Work around for:
# https://github.com/actions/runner-images/issues/13189

# Adapted from:
# https://github.com/apache/flink/blob/master/tools/azure-pipelines/free_disk_space.sh

echo "=============================================================================="
echo "Freeing up disk space on CI system"
echo "=============================================================================="

# BEFORE:
# Size: 72G
# Used: 53G
# Available: 19G

df -h
echo "Removing large directories"

sudo rm -rf /usr/share/dotnet/
sudo rm -rf /usr/local/graalvm/
sudo rm -rf /usr/local/.ghcup/
sudo rm -rf /usr/local/share/powershell
sudo rm -rf /usr/local/share/chromium
sudo rm -rf /usr/local/lib/android
sudo rm -rf /usr/local/lib/node_modules

# AFTER:
# Size: 72G
# Used: 29G
# Available: 44G

df -h

