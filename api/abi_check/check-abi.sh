#!/bin/bash

# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

set -e

#
# This script installs latest ABI tools on Linux machine
#

export INSTALL_PREFIX=/usr/local

export PATH=${INSTALL_PREFIX}/bin:$PATH

${CXX} -g -Og -I ../include -c abi_check_trace.cc -o abi_check_trace.o

abi-dumper -lver 1.0.0 abi_check_trace.o -o abi_check_trace-v1.0.0.dump
abi-dumper -lver latest abi_check_trace.o -o abi_check_trace-vlatest.dump

abi-compliance-checker -l trace --old abi_check_trace-v1.0.0.dump --new abi_check_trace-vlatest.dump

