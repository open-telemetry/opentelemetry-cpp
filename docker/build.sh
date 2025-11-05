#!/bin/bash

# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

set -o errexit
set -o nounset
set -o pipefail

Help()
{
   # Display Help
   echo "Build opentelemetry-cpp and its third-party dependencies in the selected base image"
   echo "Script options [-b|o|g|j]"
   echo
   echo "b     Base image <alpine-latest, ubuntu-latest>"
   echo "o     OpenTelemetry-cpp git tag"
   echo "h     Print Help."
   echo "j     Parallel jobs"
   echo
   echo "how to use:"
   echo
   echo "bash build.sh -b <base_image> -j ${nproc} -o <otel cpp git branch or tag>"
   echo
   echo "docker create -ti --name otel otel-cpp-<base_image> bash"
   echo "docker cp otel:/opt/opentelemetry-cpp-install ./"
   echo "docker rm -f otel"
   echo
   echo "or:"
   echo
   echo "COPY --from=otel-cpp-<base_image> /usr"
}

base_image=${base_image:="alpine-latest"}
otel_git_tag=${otel_git_tag:="main"}
cores=${cores:=1}

while getopts ":h:b:o:j:" option; do
   case $option in
    h) # display Help
         Help
         exit;;
    b) # base image
        base_image=$OPTARG
        ;;
    o) # OpenTelemetry-cpp git tag
        otel_git_tag=$OPTARG
        ;;
    j) # number of cores
        cores=$OPTARG
        ;;
    \?)
        Help
        exit;;
   esac
done

docker build -t base-${base_image}-dev -f Dockerfile.${base_image}.base .

docker build --build-arg BASE_IMAGE=${base_image} \
    --build-arg CORES=${cores} \
    --build-arg OTEL_GIT_TAG=${otel_git_tag} \
    -t otel-cpp-${base_image} -f Dockerfile .
