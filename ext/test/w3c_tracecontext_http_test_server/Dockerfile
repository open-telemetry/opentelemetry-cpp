# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

FROM python:3.14.8@sha256:b9767f56a1cc620aa72d38cc3557d26412e38a4597c92953476136b78894ccbd

RUN pip install aiohttp
RUN git clone https://github.com/w3c/trace-context

WORKDIR ./trace-context/test

ENTRYPOINT [ "python", "test.py" ]
