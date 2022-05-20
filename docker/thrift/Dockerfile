ARG BASE_IMAGE=ubuntu:latest
FROM ${BASE_IMAGE} AS thrift

ARG CORES=${nproc}
ARG THRIFT_VERSION=0.14.1

RUN mkdir mkdir -p /opt/third_party/thrift

WORKDIR /opt/third_party/th

ADD CMakeLists.txt /opt/third_party/thrift

RUN mkdir build \
    && cd build \
    && cmake -DCMAKE_INSTALL_PREFIX=/opt/third_party/install \
    -DTHRIFT_VERSION=${THRIFT_VERSION} /opt/third_party/thrift \
    && cmake --build . -j ${CORES} --target install

FROM scratch as final

COPY --from=thrift /opt/third_party/install /
