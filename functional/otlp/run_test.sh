#!/bin/bash

# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

# set -e

# To run tests in a local dev environment:
# - make sure docker is running
# - set BUILD_DIR to the top level build directory,

[ -z "${BUILD_DIR}" ] && export BUILD_DIR=$HOME/build

export CERT_DIR=../cert

export TEST_BIN_DIR=${BUILD_DIR}/functional/otlp/

${TEST_BIN_DIR}/func_otlp_http --list > test_list.txt

#
# Prepare docker image
#

docker build -t otelcpp-func-test .

echo "REPORT:" > report.log

#
# MODE 'NONE'
#

export SERVER_MODE="none"
./run_test_mode.sh

#
# MODE 'HTTP'
#

echo ""
echo "###############################################################"
echo "Starting otelcol --config otel-config-http.yaml"
echo "###############################################################"
echo ""


docker run -d \
  -v `pwd`/otel-docker-config-http.yaml:/otel-cpp/otel-config.yaml \
  -p 4318:4318 \
  --name otelcpp-test-http \
  otelcpp-func-test

sleep 5;

export SERVER_MODE="http"
./run_test_mode.sh

echo ""
echo "###############################################################"
echo "Stopping otelcol (http)"
echo "###############################################################"
echo ""

docker stop otelcpp-test-http
docker rm otelcpp-test-http

#
# MODE 'SSL'
#

echo ""
echo "###############################################################"
echo "Starting otelcol --config otel-config-https.yaml"
echo "###############################################################"
echo ""

docker run -d \
  -v `pwd`/otel-docker-config-https.yaml:/otel-cpp/otel-config.yaml \
  -v `pwd`/../cert/ca.pem:/otel-cpp/ca.pem \
  -v `pwd`/../cert/server_cert.pem:/otel-cpp/server_cert.pem \
  -v `pwd`/../cert/server_cert-key.pem:/otel-cpp/server_cert-key.pem \
  -p 4318:4318 \
  --name otelcpp-test-https \
  otelcpp-func-test

sleep 5;

export SERVER_MODE="https"
./run_test_mode.sh

echo ""
echo "###############################################################"
echo "Stopping otelcol (https)"
echo "###############################################################"
echo ""

docker stop otelcpp-test-https
docker rm otelcpp-test-https

echo ""
echo "###############################################################"
echo "TEST REPORT"
echo "###############################################################"
echo ""

cat report.log

export PASSED_COUNT=`grep -c "PASSED" report.log`
export FAILED_COUNT=`grep -c "FAILED" report.log`

echo ""
echo "###############################################################"
echo "TEST VERDICT: ${PASSED_COUNT} PASSED, ${FAILED_COUNT} FAILED"
echo "###############################################################"
echo ""

if [ ${FAILED_COUNT} != "0" ]; then
  #
  # CI FAILED
  #

  exit 1
fi;

#
# CI PASSED
#
exit 0
