#!/bin/bash

set -e

apt install -y clang-format-8 python3-pip golang git
pip3 install cmake_format==0.6.5
go get github.com/bazelbuild/buildtools/buildifier
