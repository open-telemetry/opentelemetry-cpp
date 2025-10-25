# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("@rules_cc//cc:cc_binary.bzl", "cc_binary")

cc_binary(
    name = "example_yaml",
    srcs = glob(["*.cc"]) + glob(["*.h"]),
    defines = [
        "OTEL_HAVE_OTLP_HTTP",
        "OTEL_HAVE_OTLP_GRPC",
        "OTEL_HAVE_OTLP_FILE",
        "OTEL_HAVE_ZIPKIN",
        "OTEL_HAVE_PROMETHEUS",
    ],
    tags = [
        "test",
        "yaml",
    ],
    deps = [
        "//api",
        "//examples/common/logs_foo_library:common_logs_foo_library",
        "//examples/common/metrics_foo_library:common_metrics_foo_library",
        "//exporters/ostream:ostream_log_record_exporter_builder",
        "//exporters/ostream:ostream_metric_exporter_builder",
        "//exporters/ostream:ostream_span_exporter_builder",
        "//exporters/otlp:otlp_file_log_record_exporter_builder",
        "//exporters/otlp:otlp_file_metric_exporter_builder",
        "//exporters/otlp:otlp_file_span_exporter_builder",
        "//exporters/otlp:otlp_grpc_log_record_exporter_builder",
        "//exporters/otlp:otlp_grpc_metric_exporter_builder",
        "//exporters/otlp:otlp_grpc_span_exporter_builder",
        "//exporters/otlp:otlp_http_log_record_exporter_builder",
        "//exporters/otlp:otlp_http_metric_exporter_builder",
        "//exporters/otlp:otlp_http_span_exporter_builder",
        "//exporters/prometheus:prometheus_exporter_builder",
        "//exporters/zipkin:zipkin_exporter_builder",
        "//sdk:headers",
        "//sdk/src/configuration",
        "//sdk/src/logs",
        "//sdk/src/metrics",
        "//sdk/src/trace",
        "@com_google_googletest//:gtest_main",
    ],
)
