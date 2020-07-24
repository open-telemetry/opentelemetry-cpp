#!/bin/bash

set -e

BUILD_IMAGE=opentelemetry-cpp-build
BUILD_IMAGEcpp20=opentelemetry-cpp20-build

if [[ $2 == cmake.c++20.test ]]; then
  docker image inspect "$BUILD_IMAGEcpp20" &> /dev/null || {
    docker build -t "$BUILD_IMAGEcpp20" ci/cpp20
  }
  docker run -v "$PWD":/src -w /src -it "$BUILD_IMAGEcpp20" "$@"
else
  docker image inspect "$BUILD_IMAGE" &> /dev/null || {
    docker build -t "$BUILD_IMAGE" ci
  }
  if [[ $# -ge 1 ]]; then
    docker run -v "$PWD":/src -w /src -it "$BUILD_IMAGE" "$@"
  else
    docker run -v "$PWD":/src -w /src --privileged -it "$BUILD_IMAGE" /bin/bash -l
  fi
fi