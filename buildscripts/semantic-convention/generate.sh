#!/bin/bash

# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

#
# Adapted from:
# opentelemetry-java/buildscripts/semantic-convention/generate.sh
# for opentelemetry-cpp
#

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
ROOT_DIR="${SCRIPT_DIR}/../../"

# freeze the spec & generator tools versions to make SemanticAttributes generation reproducible

# Repository up to 1.20.0:
#   https://github.com/open-telemetry/opentelemetry-specification
# Repository from 1.21.0:
#   https://github.com/open-telemetry/semantic-conventions
SEMCONV_VERSION=1.23.1

# repository: https://github.com/open-telemetry/build-tools
GENERATOR_VERSION=0.23.0

SPEC_VERSION=v$SEMCONV_VERSION
SCHEMA_URL=https://opentelemetry.io/schemas/$SEMCONV_VERSION

cd ${SCRIPT_DIR}

rm -rf tmp-semconv || true
mkdir tmp-semconv
cd tmp-semconv

git init
git remote add origin https://github.com/open-telemetry/semantic-conventions.git
git fetch origin "$SPEC_VERSION"
git reset --hard FETCH_HEAD
cd ${SCRIPT_DIR}

# echo "Help ..."

# docker run --rm otel/semconvgen:$GENERATOR_VERSION -h

echo "Generating semantic conventions for traces ..."

docker run --rm \
  -v ${SCRIPT_DIR}/tmp-semconv/model:/source \
  -v ${SCRIPT_DIR}/templates:/templates \
  -v ${ROOT_DIR}/api/include/opentelemetry/trace/:/output \
  otel/semconvgen:$GENERATOR_VERSION \
  --only span,event,attribute_group,scope \
  -f /source code \
  --template /templates/SemanticAttributes.h.j2 \
  --output /output/semantic_conventions.h \
  -Dsemconv=trace \
  -Dclass=SemanticConventions \
  -DschemaUrl=$SCHEMA_URL \
  -Dnamespace_open="namespace trace {" \
  -Dnamespace_close="}"

echo "Generating semantic conventions for resources ..."

docker run --rm \
  -v ${SCRIPT_DIR}/tmp-semconv/model:/source \
  -v ${SCRIPT_DIR}/templates:/templates \
  -v ${ROOT_DIR}/sdk/include/opentelemetry/sdk/resource/:/output \
  otel/semconvgen:$GENERATOR_VERSION \
  --only resource \
  -f /source code \
  --template /templates/SemanticAttributes.h.j2 \
  --output /output/semantic_conventions.h \
  -Dsemconv=resource \
  -Dclass=SemanticConventions \
  -DschemaUrl=$SCHEMA_URL \
  -Dnamespace_open="namespace sdk { namespace resource {" \
  -Dnamespace_close="} }"

