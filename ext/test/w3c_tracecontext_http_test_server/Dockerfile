# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

FROM python:3.15.2@sha256:86df5c86e24030d10e7e056ad0f3c5ba5d1cfb72c726bd5cc7728b5691e910bc

RUN pip install aiohttp
RUN git clone https://github.com/w3c/trace-context

WORKDIR ./trace-context/test

ENTRYPOINT [ "python", "test.py" ]
