#!/bin/bash

set -e
apt-get -y install software-properties-common
add-apt-repository -y ppa:ubuntu-toolchain-r/test
apt-get -y update
apt install -y gcc-10
apt install -y g++-10