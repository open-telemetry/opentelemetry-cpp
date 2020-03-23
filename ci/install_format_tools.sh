#!/bin/bash

set -e

apt install -y clang-format-8 python3-pip git curl
pip3 install cmake_format==0.6.5
curl -o /usr/local/bin/buildifier https://github.com/bazelbuild/buildtools/releases/tag/2.2.1/buildifier
