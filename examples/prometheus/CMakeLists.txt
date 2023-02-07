# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

include_directories(${CMAKE_SOURCE_DIR}/exporters/prometheus/include)
add_executable(prometheus_example main.cc)
target_link_libraries(
  prometheus_example ${CMAKE_THREAD_LIBS_INIT} opentelemetry_metrics
  opentelemetry_exporter_prometheus opentelemetry_resources
  common_metrics_foo_library)
