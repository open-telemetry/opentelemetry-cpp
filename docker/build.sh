#!/bin/bash

Help()
{
   # Display Help
   echo "Add description of the script functions here."
   echo
   echo "Syntax: scriptTemplate [-b|o|g|j]"
   echo "options:"
   echo "b     Base image"
   echo "o     OpenTelemetry-cpp git tag"
   echo "h     Print Help."
   echo "g     gRPC git tag"
   echo "j     Parallel jobs"
   echo
}

while getopts ":h:b:o:g:j:" option; do
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
    g) # gRPC git tag
        grpc_git_tag=$OPTARG
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

pushd grpc/
docker build --build-arg BASE_IMAGE=base-${base_image}-dev \
    --build-arg GRPC_GIT_TAG=${grpc_git_tag} \
    --build-arg CORES=${cores} \
    -t grpc-${base_image} -f Dockerfile .
popd

docker build --build-arg BASE_IMAGE=base-${base_image}-dev \
    --build-arg CORES=${cores} \
    --build-arg OTEL_GIT_TAG=${otel_git_tag} \
    -t otel-cpp-${base_image} -f Dockerfile .
