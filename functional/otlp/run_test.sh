#!/bin/bash

# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

# set -e

[ -z "${BUILD_DIR}" ] && export BUILD_DIR=$HOME/build

export CERT_DIR=../cert

export TEST_BIN_DIR=${BUILD_DIR}/functional/otlp/

${TEST_BIN_DIR}/func_otlp_http --list > test_list.txt

for T in `cat test_list.txt`
do
  echo "====================================================================="
  echo "Running test $T"
  ${TEST_BIN_DIR}/func_otlp_http --cert-dir ${CERT_DIR} ${T}
  RC=$?
  if [ ${RC} -eq 0 ]; then
    echo "TEST ${T}: PASSED"
  else
    echo "TEST ${T}: FAILED"
  fi
done
