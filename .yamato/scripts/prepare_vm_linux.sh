#!/bin/bash

set -euxo pipefail

# Setup build tools
# ===================================
export DEBIAN_FRONTEND=noninteractive
sudo -E apt remove needrestart -y #refer: https://github.com/actions/runner-images/issues/9937
sudo -E ./ci/setup_cmake.sh
sudo -E ./ci/setup_ci_environment.sh
sudo -E ./ci/setup_googletest.sh
sudo -E ./ci/install_protobuf.sh
# sudo -E ./ci/setup_grpc.sh
sudo -E apt-get install -y zip

# bee_backend requires clang compiler
.yamato/bee steve internal-unpack public 7za-linux-x64/904b71ca3f79_0d0f2cf7ea23d00d2ba5d151c735ef9f03ed99a9e451c6dcf3a198cbedf69861.zip 7z
export BEE_INTERNAL_STEVEDORE_7ZA=7z/7za
.yamato/bee steve internal-unpack public clang-centos-x64-9/9a8bc1d-7.7.1908_a100ab2e73351d22fc99337de9cbb4644f0bcfdfa0e21b7b2670573232db7539.7z TC
.yamato/bee steve internal-unpack public linux-sysroot-amd64/glibc2.17_21db091a7ba6f278ece53e4afe50f840f2ba704112aaed9562fbd86421e4ce3b.7z sysroot
echo -e '#!/bin/bash\nexec PWD/ls /clang --sysroot="PWD/sysroot" --gcc-toolchain="PWD/sysroot/usr" -I=/usr/include/c++/11 -target x86_64-glibc2.17-linux-gnu $*' > clang
sed -i -e "s|PWD|`pwd`|g" clang
chmod +x clang
