# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

FROM otel/opentelemetry-collector:0.132.2@sha256:9799f322d1b45513bf5f4e26f3c29df9f1aef7322a6a9387e9765d590057e903
COPY . .
CMD ["--config", "/otel-cpp/otel-config.yaml"]
EXPOSE 4317
EXPOSE 4318
