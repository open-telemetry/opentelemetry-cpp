#!/bin/bash

set -e

apt-get install -y software-properties-common
add-apt-repository ppa:longsleep/golang-backports
apt update

apt install -y clang-format-8 python3-pip golang-go git
pip3 install cmake_format==0.6.5
go get github.com/bazelbuild/buildtools/buildifier
