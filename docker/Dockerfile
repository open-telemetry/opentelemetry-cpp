# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

ARG BASE_IMAGE=ubuntu-latest

FROM base-${BASE_IMAGE}-dev AS otel-cpp

ARG CORES=${nproc}
ARG OTEL_GIT_TAG="main"
ARG OTEL_THIRDPARTY_TAGS_FILE="third_party_release"
ARG OTEL_CMAKE_OPTIONS_CACHE_FILE="test_common/cmake/all-options-abiv1.cmake"
ARG CXX_STANDARD=17
ARG OTEL_THIRDPARTY_BUILD_TYPE="Release"
ARG OTEL_BUILD_TYPE="Release"
ARG OTEL_BUILD_SHARED_LIBS="ON"
ARG OTEL_INSTALL_DIR="/opt/opentelemetry-cpp-install"

ENV OTEL_GIT_TAG=${OTEL_GIT_TAG}
ENV OTEL_THIRDPARTY_TAGS_FILE=${OTEL_THIRDPARTY_TAGS_FILE}
ENV OTEL_CMAKE_OPTIONS_CACHE_FILE=${OTEL_CMAKE_OPTIONS_CACHE_FILE}
ENV CXX_STANDARD=${CXX_STANDARD}
ENV OTEL_THIRDPARTY_BUILD_TYPE=${OTEL_THIRDPARTY_BUILD_TYPE}
ENV OTEL_BUILD_TYPE=${OTEL_BUILD_TYPE}
ENV OTEL_BUILD_SHARED_LIBS=${OTEL_BUILD_SHARED_LIBS}

ENV OTEL_SRC_DIR="/tmp/opentelemetry-cpp"
ENV OTEL_BUILD_DIR="/tmp/opentelemetry-cpp-build"
ENV OTEL_THIRDPARTY_BUILD_DIR="/tmp/opentelemetry-cpp-third-party-build"
ENV OTEL_INSTALL_DIR=${OTEL_INSTALL_DIR}

# clone the source
RUN git clone --recurse-submodules -j ${CORES} --depth=1 \
    -b ${OTEL_GIT_TAG} https://github.com/open-telemetry/opentelemetry-cpp.git ${OTEL_SRC_DIR}

#install third party dependencies
RUN mkdir -p ${OTEL_THIRDPARTY_BUILD_DIR} \
   && cmake \
   -S "${OTEL_SRC_DIR}/install/cmake" \
   -B "${OTEL_THIRDPARTY_BUILD_DIR}" \
   "-DCMAKE_INSTALL_PREFIX=${OTEL_INSTALL_DIR}" \
   "-DCMAKE_CXX_STANDARD=${CXX_STANDARD}" \
   "-DCMAKE_CXX_STANDARD_REQUIRED=ON" \
   "-DCMAKE_CXX_EXTENSIONS=OFF" \
   "-DCMAKE_BUILD_TYPE=${OTEL_THIRDPARTY_BUILD_TYPE}" \
   "-DOTELCPP_THIRDPARTY_TAGS_FILE=${OTEL_THIRDPARTY_TAGS_FILE}" \
   && cmake --build "${OTEL_THIRDPARTY_BUILD_DIR}" --clean-first -j ${CORES}

#install opentelemetry-cpp
RUN mkdir -p ${OTEL_BUILD_DIR} \
    && cmake \
        -S "${OTEL_SRC_DIR}" \
        -B "${OTEL_BUILD_DIR}" \
        -C "${OTEL_SRC_DIR}/${OTEL_CMAKE_OPTIONS_CACHE_FILE}" \
        "-DCMAKE_INSTALL_PREFIX=${OTEL_INSTALL_DIR}" \
        "-DCMAKE_PREFIX_PATH=${OTEL_INSTALL_DIR}" \
        "-DCMAKE_BUILD_TYPE=${OTEL_BUILD_TYPE}" \
        "-DCMAKE_CXX_STANDARD=${CXX_STANDARD}" \
        "-DCMAKE_CXX_STANDARD_REQUIRED=ON" \
        "-DCMAKE_CXX_EXTENSIONS=OFF" \
        "-DCMAKE_POSITION_INDEPENDENT_CODE=TRUE" \
        "-DBUILD_TESTING=OFF" \
        "-DBUILD_EXAMPLES=OFF" \
        "-DBUILD_SHARED_LIBS=${OTEL_BUILD_SHARED_LIBS}" \
    && cmake --build "${OTEL_BUILD_DIR}" -j ${CORES} --target install

FROM scratch AS final

ARG OTEL_INSTALL_DIR

COPY --from=otel-cpp ${OTEL_INSTALL_DIR} /
