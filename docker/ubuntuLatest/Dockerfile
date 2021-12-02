FROM ubuntu:latest
ENV DEBIAN_FRONTEND=noninteractive
WORKDIR /work

#install grpc and abseil
ARG GRPC_VERSION=v1.38.0
RUN apt-get update && apt-get install -y build-essential autoconf libtool pkg-config cmake git libssl-dev && \
    git clone --depth=1 -b $GRPC_VERSION  https://github.com/grpc/grpc.git && \
    cd grpc && git submodule update --init && \
    mkdir -p "third_party/abseil-cpp/build" && cd "third_party/abseil-cpp/build" && \
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_POSITION_INDEPENDENT_CODE=TRUE .. && \
    make -j${nproc} install && cd ../../.. && \
    mkdir build && cd build &&  \
    cmake \
        -DCMAKE_BUILD_TYPE=Release \
        -DgRPC_INSTALL=ON \
        -DgRPC_BUILD_TESTS=OFF \
        -DgRPC_ABSL_PROVIDER=package \
        -DgRPC_SSL_PROVIDER=package \
        .. && \
    make -j${nproc} install && make clean && ldconfig && \
    cd ../..

#install thrift
ARG THRIFT_VERSION=0.14.1
RUN apt-get install -y --no-install-recommends \
        libboost-all-dev \
        libevent-dev \
        wget && \
    wget https://github.com/apache/thrift/archive/refs/tags/v$THRIFT_VERSION.tar.gz && \
    tar -xvf v$THRIFT_VERSION.tar.gz && \
    mkdir -p thrift-$THRIFT_VERSION/build && cd thrift-$THRIFT_VERSION/build && \
    cmake \
        -DCMAKE_BUILD_TYPE=Release \
        -DBUILD_COMPILER=OFF \
        -DBUILD_CPP=ON \
        -DBUILD_LIBRARIES=ON \
        -DBUILD_NODEJS=OFF \
        -DBUILD_PYTHON=OFF \
        -DBUILD_JAVASCRIPT=OFF \
        -DBUILD_C_GLIB=OFF \
        -DBUILD_JAVA=OFF \
        -DBUILD_TESTING=OFF \
        -DBUILD_TUTORIALS=OFF \
        .. && \
    make -j${nproc} install && make clean && ldconfig && \
    cd ../..

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
    make -j${nproc} install && ldconfig && \
    cd ../..
