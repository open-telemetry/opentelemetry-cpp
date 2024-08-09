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

