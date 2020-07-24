FROM ubuntu:20.04

WORKDIR /setup-ci

ADD setup_ci_environment.sh /setup-ci
ADD setup_cmake.sh /setup-ci
ADD install_format_tools.sh /setup-ci

ENV DEBIAN_FRONTEND noninteractive
ENV DEBCONF_NONINTERACTIVE_SEEN true

RUN echo "tzdata tzdata/Areas select US" > /tmp/preseed.txt; \
    echo "tzdata tzdata/Zones/US select Chicago" >> /tmp/preseed.txt; \
    debconf-set-selections /tmp/preseed.txt && \
    apt-get update && \
    apt-get install -y tzdata

RUN /setup-ci/setup_ci_environment.sh \
  && /setup-ci/setup_cmake.sh \
  && /setup-ci/install_format_tools.sh
