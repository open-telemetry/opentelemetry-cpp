# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

FROM otel/opentelemetry-collector:0.129.1@sha256:9b0322c9a14fc22dde99fac9c0da97d742f4b16f725ce2a0763590c46884cdc9
COPY . .
CMD ["--config", "/otel-cpp/otel-config.yaml"]
EXPOSE 4317
EXPOSE 4318
