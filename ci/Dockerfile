# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

FROM ubuntu:18.04

WORKDIR /setup-ci

ADD setup_ci_environment.sh /setup-ci
ADD setup_cmake.sh /setup-ci
ADD install_gcc48.sh /setup-ci
ADD install_bazelisk.sh /setup-ci
ADD install_protobuf.sh /setup-ci
ADD install_format_tools.sh /setup-ci

RUN /setup-ci/setup_ci_environment.sh \
  && /setup-ci/setup_cmake.sh \
  && /setup-ci/install_gcc48.sh \
  && /setup-ci/install_bazelisk.sh \
  && /setup-ci/install_protobuf.sh \
  && /setup-ci/install_format_tools.sh
