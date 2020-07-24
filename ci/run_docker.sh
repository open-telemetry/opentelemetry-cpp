#!/bin/bash

set -e

BUILD_IMAGE=opentelemetry-cpp-build
BUILD_IMAGE2=opentelemetry-cpp-build2
docker image inspect "$BUILD_IMAGE" &> /dev/null || {
  docker build -t "$BUILD_IMAGE" ci
}

if [ $2 == cmake.c++20.test ]; then
  docker run -v "$PWD":/src -w /src -it "$BUILD_IMAGE2" "$@"
else
  if [[ $# -ge 1 ]]; then
    docker run -v "$PWD":/src -w /src -it "$BUILD_IMAGE" "$@"
  else
    docker run -v "$PWD":/src -w /src --privileged -it "$BUILD_IMAGE" /bin/bash -l
  fi
fi