#!/bin/bash

set -e

apt install -y clang-format-8 python3-pip git curl
pip3 install cmake_format==0.6.5
curl -L -o /usr/local/bin/buildifier https://github.com/bazelbuild/buildtools/releases/download/2.2.1/buildifier
chmod +x /usr/local/bin/buildifier
