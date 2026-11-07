# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

FROM otel/opentelemetry-collector:0.165.0@sha256:e3aaf15a13d3b004f1f6a236a8cee05f2935e895cb56fe6c4b7fd13634269da3
COPY . .
CMD ["--config", "/otel-cpp/otel-config.yaml"]
EXPOSE 4317
EXPOSE 4318
