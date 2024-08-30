#!/bin/bash
set -euxo pipefail

export VCPKG_ROOT="$PWD/tools/vcpkg"
export VCPKG_CMAKE="$PWD/tools/vcpkg/scripts/buildsystems/vcpkg.cmake"
export PATH=$VCPKG_ROOT:$PATH

# Build/Test/Package OpenTelemetry CPP
# ===================================
install_dir="$PWD/out"
declare -a otel_build_options=(
    # see CI "cmake.maintainer.sync.test"
    #"-DCMAKE_INSTALL_PREFIX=$install_dir"                      # Only for ninja builds
    "-DCMAKE_BUILD_TYPE=$OPENTELEMETRY_CPP_CONFIG"              # Build only release
    "-DWITH_STL=CXX17"                                          # Which version of the Standard Library for C++ to use, Matching bee_backend version
    "-DCMAKE_CXX_STANDARD=$CXX_STANDARD"                        # Use C++ Standard Language Version 17, Matching bee_backend language version
    "-DCMAKE_POSITION_INDEPENDENT_CODE=ON"                      # Add the -fPIC compiler option (off), as recommended by OpenTelemetry CPP documentation
    #"-DWITH_OTLP_GRPC=ON"                                      # Whether to include the OTLP gRPC exporter in the SDK (off), disabling it since it is apparently slow and requires additional dependencies
    "-DWITH_OTLP_HTTP=ON"                                       # Whether to include the OTLP http exporter in the SDK (off)
    #"-DWITH_OTLP_HTTP_COMPRESSION=ON"                           # Whether to include gzip compression for the OTLP http exporter in the SDK (off)
    #"-DOPENTELEMETRY_INSTALL=ON"                               # ? Whether to install opentelemetry targets (on)
    #"-DWITH_ASYNC_EXPORT_PREVIEW=OFF"                          # ? Whether to enable async export (off)
    "-DOTELCPP_MAINTAINER_MODE=OFF"                             # Build in maintainer mode (-Wall -Werror), since -Wall is not well supported by Windows STL, I'm disabling it but would rather not
    "-DWITH_NO_DEPRECATED_CODE=ON"                              # Do not include deprecated code
    "-DWITH_DEPRECATED_SDK_FACTORY=OFF"                         # Don't compile deprecated SDK Factory
    #"-DWITH_ABI_VERSION_1=OFF"                                 # ABI version 1 (on)
    #"-DWITH_ABI_VERSION_2=ON"                                  # EXPERIMENTAL: ABI version 2 preview (off)
    "-DVCPKG_TARGET_TRIPLET=$OPENTELEMETRY_CPP_LIBTYPE"         # Use static linked system dynamically linked libraries
    "-DCMAKE_TOOLCHAIN_FILE=$VCPKG_CMAKE"                       # Use vcpkg toolchain file
    "-DBUILD_TESTING=OFF"                                       # Whether to enable tests (on), makes the build faster and it does not work with x64-windows-static-md
    "-DWITH_EXAMPLES=OFF"                                       # Whether to build examples (on), makes the build faster and it does not work with x64-windows-static-md
)
cmake -S . -B build ${otel_build_options[@]}
# -GNinja
cmake --build build --parallel `getconf _NPROCESSORS_ONLN` --config $OPENTELEMETRY_CPP_CONFIG --target all
ctest --build-config $OPENTELEMETRY_CPP_CONFIG --test-dir build
cmake --install build --prefix $install_dir --config $OPENTELEMETRY_CPP_CONFIG
