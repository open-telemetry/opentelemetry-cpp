#!/bin/bash

# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

set -e

#
# This script collects ABI and compare with previous releases
# The following environment variables are expected to be set:
# - CXX : path to the compiler to use
# - ABI_VERSION : Name of the version tested (v1.2.3, latest)
# - ABI_CXX_NAME : name of the compiler (gcc12, clang13)
# - ABI_OS_NAME : name of the platform
#

export INSTALL_PREFIX=/usr/local

export PATH=${INSTALL_PREFIX}/bin:$PATH

export CXX_ABI_FLAGS="-g -Og"

for TEST in `cat ABI_CHECK_LIST`
do

echo "Compiling ${TEST}.cc with ${ABI_CXX_NAME} ..."
${CXX} ${CXX_ABI_FLAGS} -I ../include -c ${TEST}.cc -o ${TEST}.o

echo "Dumping ${TEST}.o for compiler ${ABI_CXX_NAME}, os ${ABI_OS_NAME}, version ${ABI_VERSION} ..."
abi-dumper -lver ${ABI_VERSION} ${TEST}.o -o ${TEST}-${ABI_CXX_NAME}-${ABI_OS_NAME}-${ABI_VERSION}.dump

done

#
# Do not fail CI,
# Allow to download dumps and reports for investigation
#

set +e

# TODO, compare abi

for TEST in `cat ABI_CHECK_LIST`
do

echo "Checking compliance for ${TEST} ..."

export NEW_DUMP=${TEST}-${ABI_CXX_NAME}-${ABI_OS_NAME}-${ABI_VERSION}.dump

for OLD_DUMP in `ls -1 ${TEST}-${ABI_CXX_NAME}-${ABI_OS_NAME}-*.dump`
do
  abi-compliance-checker -l ${TEST} --old ${OLD_DUMP} --new ${NEW_DUMP}
done

done


# Return OK to CI
echo "Done"
