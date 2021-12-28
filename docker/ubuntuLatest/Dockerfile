ARG BASE_IMAGE=ubuntu:latest
ARG CORES=${nproc}

FROM ${BASE_IMAGE} as base

ENV DEBIAN_FRONTEND=noninteractive
WORKDIR /work

RUN apt-get update && apt-get install -y build-essential autoconf libtool pkg-config cmake git libssl-dev

FROM base as grpc
#install grpc and abseil
ARG GRPC_VERSION=v1.38.0

ADD setup_grpc.sh .
RUN ./setup_grpc.sh

FROM base as thrift
RUN apt-get install -y --no-install-recommends wget

#install thrift
ARG THRIFT_VERSION=0.14.1
ADD setup_thrift.sh .
RUN ./setup_thrift.sh

FROM base
ARG CORES
COPY --from=grpc /usr /usr
COPY --from=thrift /usr /usr

#install opentelemetry-cpp
RUN apt-get install -y --no-install-recommends libcurl4-openssl-dev && git clone --depth=1 https://github.com/open-telemetry/opentelemetry-cpp.git && \
    cd opentelemetry-cpp && git submodule update --init && \
    mkdir -p build && cd build && \
    cmake \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_POSITION_INDEPENDENT_CODE=TRUE  \
        -DWITH_ZIPKIN=ON \
        -DWITH_JAEGER=ON \
        -DBUILD_TESTING=OFF \
        -DWITH_OTLP=ON \
        .. && \
    make -j${CORES} install && ldconfig && \
    cd ../..
