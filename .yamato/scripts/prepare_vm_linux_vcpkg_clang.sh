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

# bee_backend requires clang compiler
# ===================================
sudo -E apt-get install -y zip
export BEE_INTERNAL_STEVEDORE_7ZA=
.yamato/bee steve internal-unpack public 7za-linux-x64/904b71ca3f79_0d0f2cf7ea23d00d2ba5d151c735ef9f03ed99a9e451c6dcf3a198cbedf69861.zip 7z
export BEE_INTERNAL_STEVEDORE_7ZA=7z/7za
.yamato/bee steve internal-unpack public toolchain-llvm-centos-amd64/9.0.1-7.7.1908_9a2f0527b0c16f2524d24e372c7ebb8735e2e85ec99b6914f2a0a00c1e74d34d.7z TC
.yamato/bee steve internal-unpack public linux-sysroot-amd64/glibc2.17_21db091a7ba6f278ece53e4afe50f840f2ba704112aaed9562fbd86421e4ce3b.7z sysroot
patch --batch `pwd`/sysroot/usr/include/c++/9.1.0/variant < .yamato/variant.patch
echo -e '#!/bin/bash\nexec PWD/TC/bin/clang -B"PWD/TC/bin" --sysroot="PWD/sysroot" --gcc-toolchain="PWD/sysroot/usr" -I/usr/include/c++/9.1.0 -target x86_64-glibc2.17-linux-gnu -fuse-ld=lld $*' > clang
echo -e '#!/bin/bash\nexec PWD/TC/bin/clang++ -B"PWD/TC/bin" -std=c++17 --sysroot="PWD/sysroot" --gcc-toolchain="PWD/sysroot/usr" -I/usr/include/c++/9.1.0 -target x86_64-glibc2.17-linux-gnu $*' > clang++
sed -i -e "s|PWD|`pwd`|g" clang
sed -i -e "s|PWD|`pwd`|g" clang++
chmod +x clang
chmod +x clang++

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
