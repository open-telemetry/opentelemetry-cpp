#!/bin/bash

set -e
apt-get update
apt-get install --no-install-recommends --no-install-suggests -y \
                build-essential \
                ca-certificates \
                wget \
                git \
                g++-10
apt-get install -y lcov
