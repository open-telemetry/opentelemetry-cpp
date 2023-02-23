#!/bin/bash

# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

set -e

#
# This script collects ABI and compare with previous releases
#

export INSTALL_PREFIX=/usr/local

export PATH=${INSTALL_PREFIX}/bin:$PATH

export CXX_ABI_FLAGS="-g -Og"

echo "Compiling abi_check_baggage.cc ..."

${CXX} ${CXX_ABI_FLAGS} -I ../include -c abi_check_baggage.cc -o abi_check_baggage.o

echo "Compiling abi_check_trace.cc ..."

${CXX} ${CXX_ABI_FLAGS} -I ../include -c abi_check_trace.cc -o abi_check_trace.o

echo "Dumping abi_check_baggage.o ..."

abi-dumper -lver latest abi_check_baggage.o -o abi_check_baggage-vlatest.dump

echo "Dumping abi_check_trace.o ..."

abi-dumper -lver latest abi_check_trace.o -o abi_check_trace-vlatest.dump

#
# Do not fail CI,
# Allow to download dumps and reports for investigation
#

set +e

echo "Checking compliance for abi_check_baggage ..."

abi-compliance-checker -l baggage --old abi_check_baggage-v1.0.0.dump --new abi_check_baggage-vlatest.dump

echo "Checking compliance for abi_check_trace ..."

abi-compliance-checker -l trace --old abi_check_trace-v1.0.0.dump --new abi_check_trace-vlatest.dump

# Return OK to CI
echo "Done"
