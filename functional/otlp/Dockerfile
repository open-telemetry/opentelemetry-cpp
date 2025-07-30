# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

FROM otel/opentelemetry-collector:0.131.0@sha256:60950c82ddfdc36b757c5e193a797ebf977e70b2d1787add3f5d45cdc60a2f8d
COPY . .
CMD ["--config", "/otel-cpp/otel-config.yaml"]
EXPOSE 4317
EXPOSE 4318
