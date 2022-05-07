ARG BASE_IMAGE=ubuntu:latest
FROM ${BASE_IMAGE} AS otel-cpp

ARG BASE_IMAGE
ARG CORES=${nproc}
ARG OTEL_GIT_TAG=v1.3.0

COPY --from=grpc-${BASE_IMAGE} / /usr/local

#install opentelemetry-cpp
RUN git clone --recurse-submodules -j ${CORES} --depth=1 \
    -b ${OTEL_GIT_TAG} https://github.com/open-telemetry/opentelemetry-cpp.git \
    && cd opentelemetry-cpp \
    && mkdir -p build && cd build \
    && cmake \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_POSITION_INDEPENDENT_CODE=TRUE  \
        -DWITH_ZIPKIN=ON \
        -DWITH_JAEGER=OFF \
        -DCMAKE_INSTALL_PREFIX=/opt/third_party/install \
        -DBUILD_TESTING=OFF \
        -DWITH_OTLP=ON \
        -DBUILD_SHARED_LIBS=ON \
        .. \
    && cmake --build . --target install

FROM scratch as final

COPY --from=otel-cpp /opt/third_party/install /
