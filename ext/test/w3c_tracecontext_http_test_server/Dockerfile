# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

FROM python:3.15.2@sha256:e4bc83a3314d078d651f06c8440e9a03ba892ee2f6577437f174abd8731fb343

RUN pip install aiohttp
RUN git clone https://github.com/w3c/trace-context

WORKDIR ./trace-context/test

ENTRYPOINT [ "python", "test.py" ]
