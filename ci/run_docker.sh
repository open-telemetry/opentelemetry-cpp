#!/bin/bash

set -e

BUILD_IMAGE=opentelemetry-cpp-build
docker image inspect "$BUILD_IMAGE" &> /dev/null || {
  docker build -t "$BUILD_IMAGE" ci
}

if [[ $# -ge 1 ]]; then
  docker run -v "$PWD":/src -w /src -it "$BUILD_IMAGE" "$@"
else
  docker run -v "$PWD":/src -w /src --privileged -it "$BUILD_IMAGE" /bin/bash -l
fi
