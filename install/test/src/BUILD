# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("//:dll_deps.bzl", "dll_deps")
load("//bazel:otel_cc.bzl", "otel_cc_test")

otel_cc_test(
    name = "all_tests",
    srcs = glob(
        ["*.cc"],
        exclude = [
            "test_shims_opentracing.cc",
            "test_exporters_elasticsearch.cc",
            "test_exporters_etw.cc",
            "test_exporters_otlp_http.cc",
            "test_exporters_prometheus.cc",
            "test_exporters_zipkin.cc",
            "test_ext_http_curl.cc",
        ],
    ),
    tags = [
        "test",
    ],
    deps = dll_deps([
        "//api",
        # "//exporters/elasticsearch:es_log_record_exporter",
        # "//exporters/etw:etw_exporter",
        "//exporters/memory:in_memory_metric_exporter_factory",
        "//exporters/memory:in_memory_span_exporter",
        "//exporters/ostream:ostream_log_record_exporter",
        "//exporters/ostream:ostream_metric_exporter",
        "//exporters/ostream:ostream_span_exporter",
        "//exporters/otlp:otlp_file_exporter",
        "//exporters/otlp:otlp_file_log_record_exporter",
        "//exporters/otlp:otlp_file_metric_exporter",
        "//exporters/otlp:otlp_grpc_exporter",
        "//exporters/otlp:otlp_grpc_log_record_exporter",
        "//exporters/otlp:otlp_grpc_metric_exporter",
        # "//exporters/otlp:otlp_http_exporter",
        # "//exporters/otlp:otlp_http_log_record_exporter",
        # "//exporters/otlp:otlp_http_metric_exporter",
        # "//exporters/prometheus:prometheus_exporter",
        # "//exporters/zipkin:zipkin_exporter",
        "//sdk/src/logs",
        "//sdk/src/metrics",
        "//sdk/src/resource",
        "//sdk/src/trace",
        "//sdk/src/version",
        "@googletest//:gtest_main",
        "@nlohmann_json//:json",
        "@opentelemetry-proto//:logs_service_grpc_cc",
        "@opentelemetry-proto//:metrics_service_grpc_cc",
        "@opentelemetry-proto//:trace_service_grpc_cc",
    ]),
)
