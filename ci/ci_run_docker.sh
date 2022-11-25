#!/bin/bash

# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

set -e

format="${FORMAT:=OFF}"
thrift="${THRIFT:=OFF}"
abseil="${ABSEIL:=OFF}"
gRPC="${GRPC:=OFF}"

BUILD_IMAGE=opentelemetry-cpp-build-2004
docker image inspect "$BUILD_IMAGE" &> /dev/null || {
  docker build -t "$BUILD_IMAGE" \
    --build-arg FMT=${format} \
    --build-arg THFT=${thrift} \
    --build-arg ABSL=${abseil} \
    --build-arg GRPC=${gRPC} \
    --build-arg BASE=ubuntu:20.04 \
    ci -f ci/Dockerfile.ubuntu
}

if [[ $# -ge 1 ]]; then
  docker run --user "$(id -u):$(id -g)" -v "$PWD":/src -w /src "$BUILD_IMAGE" "$@"
else
  docker run -v "$PWD":/src -w /src --privileged -it "$BUILD_IMAGE" /bin/bash -l
fi

docker rmi --force ${BUILD_IMAGE}
