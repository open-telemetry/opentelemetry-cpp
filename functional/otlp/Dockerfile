# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

FROM otel/opentelemetry-collector:0.130.1@sha256:9d15ad63ce364bd92e385e5cfecf06200387c37a563b2fdc9396810a7403c9bd
COPY . .
CMD ["--config", "/otel-cpp/otel-config.yaml"]
EXPOSE 4317
EXPOSE 4318
