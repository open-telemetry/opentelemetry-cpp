# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

FROM otel/opentelemetry-collector:0.123.0@sha256:e5e4a13f0ea98e7ca1d1d809be040180540146888d0d764abd4e1277cba87350
COPY . .
CMD ["--config", "/otel-cpp/otel-config.yaml"]
EXPOSE 4317
EXPOSE 4318
