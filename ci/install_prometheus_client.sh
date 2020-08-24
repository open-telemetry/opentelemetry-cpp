#!/bin/bash

git clone https://github.com/jupp0r/prometheus-cpp.git
cd prometheus-cpp
git checkout v0.9.0
git submodule init
git submodule update
mkdir _build && cd _build
cmake .. -DBUILD_SHARED_LIBS=ON
make && make install
