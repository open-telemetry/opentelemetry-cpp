# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

FROM python@sha256:7e30bd51483a565ac6b2119af3f4cc179aba3ecd0b64d14cc9e64629df60d1f6

RUN pip install aiohttp
RUN git clone https://github.com/w3c/trace-context

WORKDIR ./trace-context/test

ENTRYPOINT [ "python", "test.py" ]
