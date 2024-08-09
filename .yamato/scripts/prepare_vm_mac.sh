#!/bin/bash
set -euxo pipefail

# Setup build tools
# ===================================
# https://github.com/Homebrew/homebrew-core/blob/7a574d89134ceee38d055c8f5ad3c93706bc6b3b/Formula/o/opentelemetry-cpp.rb
brew install cmake
#brew install abseil
brew install boost
brew install grpc
brew install nlohmann-json
brew install prometheus-cpp
brew install c-ares
brew install openssl@3
brew install re2
brew install curl
./ci/install_abseil.sh

# Quick Fix for mac to reuse ./ci/install_protobuf.sh
sed -i '' 's/^ldconfig/\#ldconfig/g' ./ci/install_protobuf.sh
./ci/install_protobuf.sh
