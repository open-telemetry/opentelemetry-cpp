#!/bin/bash

# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

set -e

# To run tests in a local dev environment:
# - set BUILD_DIR to the top level build directory,

[ -z "${BUILD_DIR}" ] && export BUILD_DIR="${HOME}/build"

export EXAMPLE_BIN_DIR="${BUILD_DIR}/examples/configuration/"

# Make sure `example_yaml` is in the path
export PATH=${PATH}:${EXAMPLE_BIN_DIR}

shelltest ./shelltests

