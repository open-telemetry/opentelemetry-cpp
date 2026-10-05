# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

FROM python@sha256:504679006065cdab9a85e983e269140d3c5a279492395a6863b25b1ffb377f86

RUN pip install aiohttp
RUN git clone https://github.com/w3c/trace-context

WORKDIR ./trace-context/test

ENTRYPOINT [ "python", "test.py" ]
