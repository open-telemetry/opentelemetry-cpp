# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

FROM python:3.15.0@sha256:16e7e5fc4e79fe153c818ac9b46595c7addddaad744d49595377836e300ad1c6

RUN pip install aiohttp
RUN git clone https://github.com/w3c/trace-context

WORKDIR ./trace-context/test

ENTRYPOINT [ "python", "test.py" ]
