# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

FROM otel/opentelemetry-collector:0.126.0@sha256:e02c9c8d05f282f696ae1c7122a9f06d938ce6aee78082adc35ac74bd998e3d0
COPY . .
CMD ["--config", "/otel-cpp/otel-config.yaml"]
EXPOSE 4317
EXPOSE 4318
