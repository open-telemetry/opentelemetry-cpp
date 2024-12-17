# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

FROM otel/opentelemetry-collector:0.115.1
COPY . .
CMD ["--config", "/otel-cpp/otel-config.yaml"]
EXPOSE 4318
