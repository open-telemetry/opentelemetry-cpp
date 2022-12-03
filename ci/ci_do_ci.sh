#!/bin/bash

# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

set -o errexit
set -o nounset
set -o pipefail

format="${FORMAT:=OFF}"
thrift="${THRIFT:=OFF}"
abseil="${ABSEIL:=OFF}"
gRPC="${GRPC:=OFF}"
bazel="${BAZEL:=OFF}"
BUILD_IMAGE=opentelemetry-cpp-build-2004

Help()
{
   # Display Help
   echo "Add description of the script functions here."
   echo
   echo "Syntax: scriptTemplate [-b|j]"
   echo "options:"
   echo "i     Build image"
   echo "b     With bazel"
   echo "f     With format"
   echo "t     With thrift"
   echo "a     With abseil"
   echo "g     With gRPC"
   echo "h     Print Help."
   echo "j     Parallel jobs"
   echo
}

build_image()
{
  docker image inspect "$BUILD_IMAGE" &> /dev/null || {
    docker build -t "$BUILD_IMAGE" \
      --build-arg FMT=${format} \
      --build-arg THFT=${thrift} \
      --build-arg ABSL=${abseil} \
      --build-arg GRPC=${gRPC} \
      --build-arg BZL=${bazel} \
      --build-arg BASE=ubuntu:20.04 \
      ci -f ci/Dockerfile.ubuntu
  }
}

while getopts ":iftagbj:" option; do
   case $option in
    h) # display Help
         Help
         exit;;
    i) # build image
        build_image
        exit;;
    f)
      format=ON
      ;;
    t)
      thrift=ON
      ;;
    a)
      abseil=ON
      ;;
    g)
      gRPC=ON
      ;;
    b)
      bazel=ON
      ;;
    j) # number of cores
        cores=$OPTARG
        ;;
    \?)
        Help
        exit;;
   esac
done

BAZEL_CACHE="/home/runner/.cache/bazel"

command=$@
backup="/backup/backup.tar"
container_name="otel-run"

build_image

if [[ ${bazel} == "ON" ]];
then
  docker run \
    -v $(pwd):/src \
    -v /home/runner \
    -v $(pwd):/backup \
    --tty \
    -w /src \
    --name ${container_name} \
    "$BUILD_IMAGE" bash -c \
      "cd /home/runner/ \
      && tar xf ${backup} --strip 1 &> /dev/null || { \
        echo 'no bazel cache'
      } \
      && cd /src && ${command}"

  docker run --rm --volumes-from ${container_name} \
    -v /home/runner/.cache/bazel:/backup ubuntu tar cf ${backup} /home/runner

else
  docker run \
    --user "$(id -u):$(id -g)" \
    -v $(pwd):/src \
    --tty \
    --rm \
    -w /src \
    --name ${container_name} \
    "$BUILD_IMAGE" bash -c "${command}"
fi

docker rm -f ${container_name}
docker rmi --force ${BUILD_IMAGE}
