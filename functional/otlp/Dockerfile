# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

FROM otel/opentelemetry-collector:0.160.0@sha256:0e5de4ea2076f13e8a471411403ba49f9dda49f4e07962cad4f380fc7e667f01
COPY . .
CMD ["--config", "/otel-cpp/otel-config.yaml"]
EXPOSE 4317
EXPOSE 4318
