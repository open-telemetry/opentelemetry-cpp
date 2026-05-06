#!/bin/bash

# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

set -ex

PKG_CONFIG_PATH="/usr/local/lib64/pkgconfig:${PKG_CONFIG_PATH:-}"

validate_package() {
  local package=$1
  if pkg-config --exists "${package}" 2>/dev/null; then
    pkg-config --validate "${package}" --print-errors
  fi
}

# Core packages
for library in api common logs metrics resources trace version; do
  pkg-config --validate opentelemetry_${library} --print-errors
done

# ostream exporter
for variant in span span_builder metrics metrics_builder logs logs_builder; do
  validate_package "opentelemetry_exporter_ostream_${variant}"
done

# OTLP exporters
validate_package "opentelemetry_otlp_recordable"
validate_package "opentelemetry_exporter_otlp_builder_utils"

# OTLP gRPC exporter
validate_package "opentelemetry_exporter_otlp_grpc"
for variant in client builder log log_builder metrics metric_builder; do
  validate_package "opentelemetry_exporter_otlp_grpc_${variant}"
done

# OTLP HTTP and File exporters
for protocol in http file; do
  validate_package "opentelemetry_exporter_otlp_${protocol}"
  for variant in client builder log log_builder metric metric_builder; do
    validate_package "opentelemetry_exporter_otlp_${protocol}_${variant}"
  done
done

# Other exporters
for exporter in prometheus prometheus_builder zipkin_trace elasticsearch_logs in_memory in_memory_metric; do
  validate_package "opentelemetry_exporter_${exporter}"
done

# Optional features
validate_package "opentelemetry_configuration"
