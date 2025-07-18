#!/bin/bash

# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

set -e

# To run tests in a local dev environment:
# - make sure docker is running
# - set BUILD_DIR to the top level build directory,

[ -z "${BUILD_DIR}" ] && export BUILD_DIR="${HOME}/build"

export TEST_BIN_DIR="${BUILD_DIR}/functional/configuration/"

export PATH=${PATH}:${TEST_BIN_DIR}

which otel_configuration_check

otel_configuration_check --help

which shelltest

shelltest --version

shelltest --help

shelltest ./shelltests

