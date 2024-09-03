#!/bin/bash

set -euxo pipefail

export DEBIAN_FRONTEND=noninteractive
sudo -E apt-get update -y
sudo -E apt remove needrestart -y #refer: https://github.com/actions/runner-images/issues/9937

# Install CMake and build tools
# ===================================
sudo -E apt-get install -y zip bc
sudo -E ./ci/setup_ci_environment.sh
sudo -E ./tools/setup-cmake.sh

# Install vcpkg
# ===================================
export VCPKG_ROOT="$PWD/tools/vcpkg"
export VCPKG_CMAKE="$PWD/tools/vcpkg/scripts/buildsystems/vcpkg.cmake"
pushd $VCPKG_ROOT
bash $PWD/scripts/bootstrap.sh -disableMetrics
export PATH=$PWD:$PATH
./vcpkg integrate install
popd # VCPKG_ROOT

# Setup build tools
# ===================================
declare -a vcpkg_dependencies=(
    "gtest:$OPENTELEMETRY_CPP_LIBTYPE"
    "benchmark:$OPENTELEMETRY_CPP_LIBTYPE"
    "protobuf:$OPENTELEMETRY_CPP_LIBTYPE"
    #"ms-gsl:$OPENTELEMETRY_CPP_LIBTYPE"
    "nlohmann-json:$OPENTELEMETRY_CPP_LIBTYPE"
    #"abseil:$OPENTELEMETRY_CPP_LIBTYPE"
    #"gRPC:$OPENTELEMETRY_CPP_LIBTYPE"
    #"prometheus-cpp:$OPENTELEMETRY_CPP_LIBTYPE"
    "curl:$OPENTELEMETRY_CPP_LIBTYPE"
    #"thrift:$OPENTELEMETRY_CPP_LIBTYPE"
)
for dep in "${vcpkg_dependencies[@]}"; do
    if [[ $dep == protobuf:* ]] || [[ $dep == benchmark:* ]] ; then
        vcpkg "--vcpkg-root=${VCPKG_ROOT}" install --overlay-ports=$VCPKG_ROOT/ports $dep;
    else
        vcpkg "--vcpkg-root=${VCPKG_ROOT}" install $dep;
    fi
done
