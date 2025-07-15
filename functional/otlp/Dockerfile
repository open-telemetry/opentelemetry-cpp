# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

FROM otel/opentelemetry-collector:0.130.0@sha256:0c066d4388070dad8dc9961d9f23649e85a226620e6b359334e4a6c7f9d73b23
COPY . .
CMD ["--config", "/otel-cpp/otel-config.yaml"]
EXPOSE 4317
EXPOSE 4318
