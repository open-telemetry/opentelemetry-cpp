#!/bin/bash

# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

set -e

# To run tests in a local dev environment:
# - make sure docker is running
# - set BUILD_DIR to the top level build directory,

[ -z "${BUILD_DIR}" ] && export BUILD_DIR="${HOME}/build"

export EXAMPLE_BIN_DIR="${BUILD_DIR}/example/configuration/"

export PATH=${PATH}:${EXAMPLE_BIN_DIR}

shelltest ./shelltests

