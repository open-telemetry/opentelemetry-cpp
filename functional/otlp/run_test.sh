#!/bin/bash

# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

# set -e

[ -z "${BUILD_DIR}" ] && export BUILD_DIR=$HOME/build

export CERT_DIR=../cert

export TEST_BIN_DIR=${BUILD_DIR}/functional/otlp/

${TEST_BIN_DIR}/func_otlp_http --list > test_list.txt

#
# MODE 'NONE'
#

export SERVER_MODE="none"
./run_test_mode.sh

#
# MODE 'HTTP'
#

echo "FIXME: start otelcol --config otel-config-http.yaml"

export SERVER_MODE="http"
# ./run_test_mode.sh

echo "FIXME: stop otelcol"

#
# MODE 'SSL'
#

echo "FIXME: start otelcol --config otel-config-https.yaml"

export SERVER_MODE="http"
# ./run_test_mode.sh

echo "FIXME: stop otelcol"

