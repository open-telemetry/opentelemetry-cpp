# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

FROM otel/opentelemetry-collector:0.128.0@sha256:580cc87f5ea6c9e3969c37f3c5394b7a8385abb8189df0ddc63b9575a00f52fc
COPY . .
CMD ["--config", "/otel-cpp/otel-config.yaml"]
EXPOSE 4317
EXPOSE 4318
