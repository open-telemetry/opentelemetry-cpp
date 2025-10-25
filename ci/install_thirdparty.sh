#!/bin/bash

# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

set -e

usage() {
    echo "Usage: $0 --install-dir <path> [--tags-file <file>] [--packages \"<pkg1>;<pkg2>\"]"
    echo "  --install-dir <path>          Path where third-party packages will be installed (required)"
    echo "  --tags-file <file>            File containing tags for third-party packages (optional)"
    echo "  --packages \"<pkg1>;<pkg2>;...\"  Semicolon-separated list of packages to build (optional). Default installs all third-party packages."
    echo "  -h, --help                    Show this help message"
}

THIRDPARTY_TAGS_FILE=""
THIRDPARTY_PACKAGES=""
SRC_DIR="$(pwd)"
THIRDPARTY_INSTALL_DIR=""

while [[ $# -gt 0 ]]; do
    case $1 in
        --install-dir)
            if [ -z "$2" ] || [[ "$2" == --* ]]; then
                echo "Error: --install-dir requires a value" >&2
                usage
                exit 1
            fi
            THIRDPARTY_INSTALL_DIR="$2"
            shift 2
            ;;
        --tags-file)
            if [ -z "$2" ] || [[ "$2" == --* ]]; then
                echo "Error: --tags-file requires a value" >&2
                usage
                exit 1
            fi
            THIRDPARTY_TAGS_FILE="$2"
            shift 2
            ;;
        --packages)
            if [ -z "$2" ] || [[ "$2" == --* ]]; then
                echo "Error: --packages requires a value" >&2
                usage
                exit 1
            fi
            THIRDPARTY_PACKAGES="$2"
            shift 2
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            echo "Unknown option: $1" >&2
            usage
            exit 1
            ;;
    esac
done

if [ -z "${THIRDPARTY_INSTALL_DIR}" ]; then
    echo "Error: --install-dir is a required argument." >&2
    usage
    exit 1
fi

if [ -z "${CXX_STANDARD}" ]; then
    CXX_STANDARD=17
fi

THIRDPARTY_BUILD_DIR="/tmp/otel-cpp-third-party-build"

if [ -d "${THIRDPARTY_BUILD_DIR}" ]; then
    rm -rf "${THIRDPARTY_BUILD_DIR}"
fi

cmake -S "${SRC_DIR}/install/cmake" -B "${THIRDPARTY_BUILD_DIR}" \
   "-DCMAKE_INSTALL_PREFIX=${THIRDPARTY_INSTALL_DIR}" \
   "-DCMAKE_CXX_STANDARD=${CXX_STANDARD}" \
   "-DOTELCPP_THIRDPARTY_TAGS_FILE=${THIRDPARTY_TAGS_FILE}" \
   "-DOTELCPP_PROTO_PATH=${OTELCPP_PROTO_PATH}" \
   "-DOTELCPP_THIRDPARTY_INSTALL_LIST=${THIRDPARTY_PACKAGES}"

cmake --build "${THIRDPARTY_BUILD_DIR}" --clean-first -j"$(nproc)"

if [ "${THIRDPARTY_INSTALL_DIR}" = "/usr/local" ]; then
  ldconfig
fi

echo "Third-party packages installed successfully."
echo "-- THIRDPARTY_INSTALL_DIR: ${THIRDPARTY_INSTALL_DIR}"
echo "-- THIRDPARTY_TAGS_FILE: ${THIRDPARTY_TAGS_FILE}"
echo "-- THIRDPARTY_PACKAGES: ${THIRDPARTY_PACKAGES:-all}"
echo "-- CXX_STANDARD: ${CXX_STANDARD}"
