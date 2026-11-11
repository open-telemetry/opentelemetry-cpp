# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

FROM python:3.15.2@sha256:d650864079477d7468a0f384777be3b534f2bbdf5beb6155dbd73a4723b89300

RUN pip install aiohttp
RUN git clone https://github.com/w3c/trace-context

WORKDIR ./trace-context/test

ENTRYPOINT [ "python", "test.py" ]
