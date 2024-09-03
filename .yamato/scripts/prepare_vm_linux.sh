#!/bin/bash

set -euxo pipefail

export DEBIAN_FRONTEND=noninteractive
sudo -E apt-get update -y
sudo -E apt remove needrestart -y #refer: https://github.com/actions/runner-images/issues/9937

# ===================================
sudo -E apt-get install -y zip

# Setup build tools
# ===================================
sudo -E apt-get install -y zlib1g-dev bc
# sudo -E ./tools/setup-ninja.sh
sudo -E ./ci/setup_ci_environment.sh
sudo -E ./tools/setup-cmake.sh
sudo -E ./ci/setup_googletest.sh
sudo -E ./ci/install_protobuf.sh
sudo -E ./ci/install_abseil.sh
# sudo -E ./ci/setup_grpc.sh
