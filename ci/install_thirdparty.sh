#!/bin/bash

# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

set -e

usage() {
    echo "Usage: $0 --install-dir <path> [--tags-file <file>] [--packages \"<pkg1>;<pkg2>\"] [--build-type <type>]"
    echo "  --install-dir <path>          Path where third-party packages will be installed (required)"
    echo "  --tags-file <file>            File containing tags for third-party packages (optional)"
    echo "  --packages \"<pkg1>;<pkg2>;...\"  Semicolon-separated list of packages to build (optional). Default installs all third-party packages."
    echo "  --build-type <type>           Build type for third-party packages (optional). Valid: Debug|Release|RelWithDebInfo|MinSizeRel. Default: Release"
    echo "  -h, --help                    Show this help message"
}

THIRDPARTY_TAGS_FILE=""
THIRDPARTY_PACKAGES=""
SRC_DIR="$(pwd)"
THIRDPARTY_INSTALL_DIR=""
BUILD_TYPE=""

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
        --build-type)
            if [ -z "$2" ] || [[ "$2" == --* ]]; then
                echo "Error: --build-type requires a value" >&2
                usage
                exit 1
            fi
            BUILD_TYPE="$2"
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

if [[ "${BUILD_TYPE}" =~ ^(Debug|Release|RelWithDebInfo|MinSizeRel)$ ]]; then
    CMAKE_BUILD_TYPE="${BUILD_TYPE}"
else
    if [ -n "${BUILD_TYPE}" ]; then
        echo "Warning: invalid --build-type '${BUILD_TYPE}', defaulting to Release" >&2
    fi
    CMAKE_BUILD_TYPE=Release
fi

THIRDPARTY_BUILD_DIR="/tmp/otel-cpp-third-party-build"
CMAKE_OPTIONS=(
   "-DCMAKE_INSTALL_PREFIX=${THIRDPARTY_INSTALL_DIR}"
   "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
   "-DCMAKE_CXX_STANDARD=${CXX_STANDARD}"
   "-DOTELCPP_THIRDPARTY_TAGS_FILE=${THIRDPARTY_TAGS_FILE}"
   "-DOTELCPP_PROTO_PATH=${OTELCPP_PROTO_PATH}"
   "-DOTELCPP_THIRDPARTY_INSTALL_LIST=${THIRDPARTY_PACKAGES}"
)
CMAKE_BUILD_ARGS=(--clean-first --parallel)

shopt -s nocasematch
if [[ "${OTELCPP_CMAKE_VERBOSE_BUILD:-OFF}" =~ ^(1|ON|TRUE|YES)$ ]]; then
    CMAKE_BUILD_ARGS+=(--verbose)
fi
shopt -u nocasematch

if command -v ninja >/dev/null 2>&1; then
    CMAKE_OPTIONS+=("-G" "Ninja")
fi

if [ -d "${THIRDPARTY_BUILD_DIR}" ]; then
    rm -rf "${THIRDPARTY_BUILD_DIR}"
fi

cmake -S "${SRC_DIR}/install/cmake" -B "${THIRDPARTY_BUILD_DIR}" \
    "${CMAKE_OPTIONS[@]}"

cmake --build "${THIRDPARTY_BUILD_DIR}" "${CMAKE_BUILD_ARGS[@]}"

if [ "${THIRDPARTY_INSTALL_DIR}" = "/usr/local" ]; then
  ldconfig
fi

echo "Third-party packages installed successfully."
echo "-- THIRDPARTY_INSTALL_DIR: ${THIRDPARTY_INSTALL_DIR}"
echo "-- THIRDPARTY_TAGS_FILE: ${THIRDPARTY_TAGS_FILE}"
echo "-- THIRDPARTY_PACKAGES: ${THIRDPARTY_PACKAGES:-all}"
echo "-- CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}"
echo "-- CXX_STANDARD: ${CXX_STANDARD}"
