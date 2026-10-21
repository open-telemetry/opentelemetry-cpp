# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

FROM otel/opentelemetry-collector:0.163.1@sha256:b3bd84db789cec2e6839e1bc3b99d8d07c69fd0b3e61cc6d77ca4e90453beabf
COPY . .
CMD ["--config", "/otel-cpp/otel-config.yaml"]
EXPOSE 4317
EXPOSE 4318
