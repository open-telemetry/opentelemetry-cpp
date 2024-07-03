# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("//bazel:otel_cc_benchmark.bzl", "otel_cc_benchmark")
load("//:dll_deps.bzl", "dll_deps", "avoid_dll_lock")
load("//bazel:otel_cc.bzl", "otel_cc_library", "otel_cc_binary", "otel_cc_test", "otel_cc_import", "otel_cc_shared_library")

avoid_dll_lock()

package(default_visibility = ["//visibility:public"])

otel_cc_library(
    name = "otlp_recordable",
    srcs = [
        "src/otlp_environment.cc",
        "src/otlp_log_recordable.cc",
        "src/otlp_metric_utils.cc",
        "src/otlp_populate_attribute_utils.cc",
        "src/otlp_recordable.cc",
        "src/otlp_recordable_utils.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/otlp/otlp_environment.h",
        "include/opentelemetry/exporters/otlp/otlp_log_recordable.h",
        "include/opentelemetry/exporters/otlp/otlp_metric_utils.h",
        "include/opentelemetry/exporters/otlp/otlp_populate_attribute_utils.h",
        "include/opentelemetry/exporters/otlp/otlp_preferred_temporality.h",
        "include/opentelemetry/exporters/otlp/otlp_recordable.h",
        "include/opentelemetry/exporters/otlp/otlp_recordable_utils.h",
        "include/opentelemetry/exporters/otlp/protobuf_include_prefix.h",
        "include/opentelemetry/exporters/otlp/protobuf_include_suffix.h",
    ],
    strip_include_prefix = "include",
    tags = ["otlp"],
    deps = [
        "//sdk/src/logs",
        "//sdk/src/resource",
        "//sdk/src/trace",
        "@opentelemetry-proto//:logs_service_proto_cc",
        "@opentelemetry-proto//:metrics_service_proto_cc",
        "@opentelemetry-proto//:trace_service_proto_cc",
    ],
)

otel_cc_library(
    name = "otlp_grpc_client",
    srcs = [
        "src/otlp_grpc_client.cc",
        "src/otlp_grpc_utils.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/otlp/otlp_environment.h",
        "include/opentelemetry/exporters/otlp/otlp_grpc_client.h",
        "include/opentelemetry/exporters/otlp/otlp_grpc_client_options.h",
        "include/opentelemetry/exporters/otlp/otlp_grpc_utils.h",
        "include/opentelemetry/exporters/otlp/protobuf_include_prefix.h",
        "include/opentelemetry/exporters/otlp/protobuf_include_suffix.h",
    ],
    strip_include_prefix = "include",
    tags = [
        "otlp",
        "otlp_grpc",
    ],
    deps = [
        "//ext:headers",
        "//sdk/src/common:global_log_handler",
        "@@grpc~//:grpc++",
        "@opentelemetry-proto//:common_proto_cc",
        "@opentelemetry-proto//:logs_service_grpc_cc",
        "@opentelemetry-proto//:metrics_service_grpc_cc",
        "@opentelemetry-proto//:trace_service_grpc_cc",
    ],
)

otel_cc_library(
    name = "otlp_grpc_exporter",
    srcs = [
        "src/otlp_grpc_exporter.cc",
        "src/otlp_grpc_exporter_factory.cc",
        "src/otlp_grpc_exporter_options.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/otlp/otlp_environment.h",
        "include/opentelemetry/exporters/otlp/otlp_grpc_client_options.h",
        "include/opentelemetry/exporters/otlp/otlp_grpc_exporter.h",
        "include/opentelemetry/exporters/otlp/otlp_grpc_exporter_factory.h",
        "include/opentelemetry/exporters/otlp/otlp_grpc_exporter_options.h",
        "include/opentelemetry/exporters/otlp/otlp_grpc_utils.h",
        "include/opentelemetry/exporters/otlp/protobuf_include_prefix.h",
        "include/opentelemetry/exporters/otlp/protobuf_include_suffix.h",
    ],
    strip_include_prefix = "include",
    tags = [
        "otlp",
        "otlp_grpc",
    ],
    deps = [
        ":otlp_recordable",
        ":otlp_grpc_client",
        "//ext:headers",
        "//sdk/src/trace",

        # For gRPC
        "@opentelemetry-proto//:trace_service_grpc_cc",
    ],
)

otel_cc_library(
    name = "otlp_http_client",
    srcs = [
        "src/otlp_http.cc",
        "src/otlp_http_client.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/otlp/otlp_environment.h",
        "include/opentelemetry/exporters/otlp/otlp_http.h",
        "include/opentelemetry/exporters/otlp/otlp_http_client.h",
        "include/opentelemetry/exporters/otlp/protobuf_include_prefix.h",
        "include/opentelemetry/exporters/otlp/protobuf_include_suffix.h",
    ],
    copts = [
        "-DCURL_STATICLIB",
    ],
    linkopts = select({
        "//bazel:windows": [
            "-DEFAULTLIB:advapi32.lib",
            "-DEFAULTLIB:crypt32.lib",
        ],
        "//conditions:default": [],
    }),
    strip_include_prefix = "include",
    tags = [
        "otlp",
        "otlp_http",
        "otlp_http_log",
    ],
    deps = [
        "//api",
        "//ext/src/http/client/curl:http_client_curl",
        "//sdk:headers",
        "//sdk/src/common:base64",
        "@opentelemetry-proto//:common_proto_cc",
        "@@abseil-cpp~//absl/strings",
        "@nlohmann_json//:json",
    ],
)

otel_cc_library(
    name = "otlp_http_exporter",
    srcs = [
        "src/otlp_http_exporter.cc",
        "src/otlp_http_exporter_factory.cc",
        "src/otlp_http_exporter_options.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/otlp/otlp_environment.h",
        "include/opentelemetry/exporters/otlp/otlp_http_exporter.h",
        "include/opentelemetry/exporters/otlp/otlp_http_exporter_factory.h",
        "include/opentelemetry/exporters/otlp/otlp_http_exporter_options.h",
        "include/opentelemetry/exporters/otlp/protobuf_include_prefix.h",
        "include/opentelemetry/exporters/otlp/protobuf_include_suffix.h",
    ],
    strip_include_prefix = "include",
    tags = [
        "otlp",
        "otlp_http",
    ],
    deps = [
        ":otlp_http_client",
        ":otlp_recordable",
        "//sdk/src/trace",
        "@opentelemetry-proto//:trace_service_proto_cc",
    ],
)

otel_cc_library(
    name = "otlp_file_client",
    srcs = [
        "src/otlp_file_client.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/otlp/otlp_environment.h",
        "include/opentelemetry/exporters/otlp/otlp_file_client.h",
        "include/opentelemetry/exporters/otlp/otlp_file_client_options.h",
        "include/opentelemetry/exporters/otlp/protobuf_include_prefix.h",
        "include/opentelemetry/exporters/otlp/protobuf_include_suffix.h",
    ],
    strip_include_prefix = "include",
    tags = [
        "otlp",
        "otlp_file",
    ],
    deps = [
        "//api",
        "//sdk:headers",
        "//sdk/src/common:base64",
        "@opentelemetry-proto//:common_proto_cc",
        "@@abseil-cpp~//absl/strings",
        "@nlohmann_json//:json",
    ],
)

otel_cc_library(
    name = "otlp_file_exporter",
    srcs = [
        "src/otlp_file_exporter.cc",
        "src/otlp_file_exporter_factory.cc",
        "src/otlp_file_exporter_options.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/otlp/otlp_environment.h",
        "include/opentelemetry/exporters/otlp/otlp_file_exporter.h",
        "include/opentelemetry/exporters/otlp/otlp_file_exporter_factory.h",
        "include/opentelemetry/exporters/otlp/otlp_file_exporter_options.h",
        "include/opentelemetry/exporters/otlp/protobuf_include_prefix.h",
        "include/opentelemetry/exporters/otlp/protobuf_include_suffix.h",
    ],
    strip_include_prefix = "include",
    tags = [
        "otlp",
        "otlp_file",
    ],
    deps = [
        ":otlp_file_client",
        ":otlp_recordable",
        "//sdk/src/trace",
        "@opentelemetry-proto//:trace_service_proto_cc",
    ],
)

otel_cc_library(
    name = "otlp_grpc_metric_exporter",
    srcs = [
        "src/otlp_grpc_metric_exporter.cc",
        "src/otlp_grpc_metric_exporter_factory.cc",
        "src/otlp_grpc_metric_exporter_options.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/otlp/otlp_environment.h",
        "include/opentelemetry/exporters/otlp/otlp_grpc_client_options.h",
        "include/opentelemetry/exporters/otlp/otlp_grpc_metric_exporter.h",
        "include/opentelemetry/exporters/otlp/otlp_grpc_metric_exporter_factory.h",
        "include/opentelemetry/exporters/otlp/otlp_grpc_metric_exporter_options.h",
        "include/opentelemetry/exporters/otlp/protobuf_include_prefix.h",
        "include/opentelemetry/exporters/otlp/protobuf_include_suffix.h",
    ],
    strip_include_prefix = "include",
    tags = [
        "otlp",
        "otlp_grpc_metric",
    ],
    deps = [
        ":otlp_recordable",
        ":otlp_grpc_client",
        "//ext:headers",
        "//sdk/src/metrics",

        # For gRPC
        "@opentelemetry-proto//:metrics_service_grpc_cc",
    ],
)

otel_cc_library(
    name = "otlp_http_metric_exporter",
    srcs = [
        "src/otlp_http_metric_exporter.cc",
        "src/otlp_http_metric_exporter_factory.cc",
        "src/otlp_http_metric_exporter_options.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/otlp/otlp_environment.h",
        "include/opentelemetry/exporters/otlp/otlp_http_metric_exporter.h",
        "include/opentelemetry/exporters/otlp/otlp_http_metric_exporter_factory.h",
        "include/opentelemetry/exporters/otlp/otlp_http_metric_exporter_options.h",
        "include/opentelemetry/exporters/otlp/protobuf_include_prefix.h",
        "include/opentelemetry/exporters/otlp/protobuf_include_suffix.h",
    ],
    strip_include_prefix = "include",
    tags = [
        "otlp",
        "otlp_http_metric",
    ],
    deps = [
        ":otlp_http_client",
        ":otlp_recordable",
        "//sdk/src/metrics",
        "@opentelemetry-proto//:metrics_service_proto_cc",
    ],
)

otel_cc_library(
    name = "otlp_file_metric_exporter",
    srcs = [
        "src/otlp_file_metric_exporter.cc",
        "src/otlp_file_metric_exporter_factory.cc",
        "src/otlp_file_metric_exporter_options.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/otlp/otlp_environment.h",
        "include/opentelemetry/exporters/otlp/otlp_file_metric_exporter.h",
        "include/opentelemetry/exporters/otlp/otlp_file_metric_exporter_factory.h",
        "include/opentelemetry/exporters/otlp/otlp_file_metric_exporter_options.h",
        "include/opentelemetry/exporters/otlp/protobuf_include_prefix.h",
        "include/opentelemetry/exporters/otlp/protobuf_include_suffix.h",
    ],
    strip_include_prefix = "include",
    tags = [
        "otlp",
        "otlp_file_metric",
    ],
    deps = [
        ":otlp_file_client",
        ":otlp_recordable",
        "//sdk/src/metrics",
        "@opentelemetry-proto//:metrics_service_proto_cc",
    ],
)

otel_cc_library(
    name = "otlp_http_log_record_exporter",
    srcs = [
        "src/otlp_http_log_record_exporter.cc",
        "src/otlp_http_log_record_exporter_factory.cc",
        "src/otlp_http_log_record_exporter_options.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/otlp/otlp_environment.h",
        "include/opentelemetry/exporters/otlp/otlp_http_log_record_exporter.h",
        "include/opentelemetry/exporters/otlp/otlp_http_log_record_exporter_factory.h",
        "include/opentelemetry/exporters/otlp/otlp_http_log_record_exporter_options.h",
        "include/opentelemetry/exporters/otlp/protobuf_include_prefix.h",
        "include/opentelemetry/exporters/otlp/protobuf_include_suffix.h",
    ],
    strip_include_prefix = "include",
    tags = [
        "otlp",
        "otlp_http_log",
    ],
    deps = [
        ":otlp_http_client",
        ":otlp_recordable",
        "//sdk/src/logs",
        "@opentelemetry-proto//:logs_service_proto_cc",
    ],
)

otel_cc_library(
    name = "otlp_file_log_record_exporter",
    srcs = [
        "src/otlp_file_log_record_exporter.cc",
        "src/otlp_file_log_record_exporter_factory.cc",
        "src/otlp_file_log_record_exporter_options.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/otlp/otlp_environment.h",
        "include/opentelemetry/exporters/otlp/otlp_file_log_record_exporter.h",
        "include/opentelemetry/exporters/otlp/otlp_file_log_record_exporter_factory.h",
        "include/opentelemetry/exporters/otlp/otlp_file_log_record_exporter_options.h",
        "include/opentelemetry/exporters/otlp/protobuf_include_prefix.h",
        "include/opentelemetry/exporters/otlp/protobuf_include_suffix.h",
    ],
    strip_include_prefix = "include",
    tags = [
        "otlp",
        "otlp_file_log",
    ],
    deps = [
        ":otlp_file_client",
        ":otlp_recordable",
        "//sdk/src/logs",
        "@opentelemetry-proto//:logs_service_proto_cc",
    ],
)

otel_cc_library(
    name = "otlp_grpc_log_record_exporter",
    srcs = [
        "src/otlp_grpc_log_record_exporter.cc",
        "src/otlp_grpc_log_record_exporter_factory.cc",
        "src/otlp_grpc_log_record_exporter_options.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/otlp/otlp_environment.h",
        "include/opentelemetry/exporters/otlp/otlp_grpc_client_options.h",
        "include/opentelemetry/exporters/otlp/otlp_grpc_log_record_exporter.h",
        "include/opentelemetry/exporters/otlp/otlp_grpc_log_record_exporter_factory.h",
        "include/opentelemetry/exporters/otlp/otlp_grpc_log_record_exporter_options.h",
        "include/opentelemetry/exporters/otlp/protobuf_include_prefix.h",
        "include/opentelemetry/exporters/otlp/protobuf_include_suffix.h",
    ],
    strip_include_prefix = "include",
    tags = [
        "otlp",
        "otlp_grpc_log",
    ],
    deps = [
        ":otlp_recordable",
        ":otlp_grpc_client",
        "//ext:headers",
        "//sdk/src/logs",
        # For gRPC
        "@opentelemetry-proto//:logs_service_grpc_cc",
    ],
)

otel_cc_test(
    name = "otlp_recordable_test",
    srcs = ["test/otlp_recordable_test.cc"],
    tags = [
        "otlp",
        "test",
    ],
    deps = dll_deps([
        ":otlp_recordable",
        ":otlp_grpc_client",
        "@googletest//:gtest_main",
        "@opentelemetry-proto//:trace_service_proto_cc",
        "@opentelemetry-proto//:logs_service_grpc_cc",
    ]),
)

otel_cc_test(
    name = "otlp_metrics_serialization_test",
    srcs = ["test/otlp_metrics_serialization_test.cc"],
    tags = [
        "otlp",
        "test",
    ],
    deps = dll_deps([
        ":otlp_recordable",
        "@opentelemetry-proto//:metrics_service_proto_cc",
        "@googletest//:gtest_main",
    ]),
)

otel_cc_test(
    name = "otlp_log_recordable_test",
    srcs = [
        "test/otlp_log_recordable_test.cc",
    ],
    tags = [
        "otlp",
        "test",
    ],
    deps = dll_deps([
        ":otlp_recordable",
        "@opentelemetry-proto//:logs_service_proto_cc",
        "@googletest//:gtest_main",
    ]),
)

otel_cc_test(
    name = "otlp_file_client_test",
    srcs = ["test/otlp_file_client_test.cc"],
    tags = [
        "otlp",
        "otlp_file",
        "test",
    ],
    deps = dll_deps([
        ":otlp_file_client",
        ":otlp_file_exporter",
        ":otlp_recordable",
        "//api",
        "@googletest//:gtest_main",
        "@opentelemetry-proto//:trace_service_proto_cc",
        "@opentelemetry-proto//:logs_service_proto_cc",
        "@nlohmann_json//:json",
    ]),
)

otel_cc_test(
    name = "otlp_grpc_exporter_test",
    srcs = ["test/otlp_grpc_exporter_test.cc"],
    tags = [
        "otlp",
        "otlp_grpc",
        "test",
    ],
    deps = dll_deps([
        ":otlp_grpc_exporter",
        "//api",
        "@googletest//:gtest_main",
    ]),
)

otel_cc_test(
    name = "otlp_grpc_exporter_factory_test",
    srcs = ["test/otlp_grpc_exporter_factory_test.cc"],
    tags = [
        "otlp",
        "otlp_grpc",
        "test",
    ],
    deps = dll_deps([
        ":otlp_grpc_exporter",
        "//api",
        "@googletest//:gtest_main",
    ]),
)

otel_cc_test(
    name = "otlp_http_exporter_test",
    srcs = ["test/otlp_http_exporter_test.cc"],
    tags = [
        "otlp",
        "otlp_http",
        "test",
    ],
    deps = dll_deps([
        ":otlp_http_exporter",
        "//api",
        "//test_common/src/http/client/nosend:http_client_nosend",
        "@googletest//:gtest_main",
    ]),
)

otel_cc_test(
    name = "otlp_http_exporter_factory_test",
    srcs = ["test/otlp_http_exporter_factory_test.cc"],
    tags = [
        "otlp",
        "otlp_http",
        "test",
    ],
    deps = dll_deps([
        ":otlp_http_exporter",
        "//api",
        "//test_common/src/http/client/nosend:http_client_nosend",
        "@googletest//:gtest_main",
    ]),
)

otel_cc_test(
    name = "otlp_file_exporter_test",
    srcs = ["test/otlp_file_exporter_test.cc"],
    tags = [
        "otlp",
        "otlp_file",
        "test",
    ],
    deps = dll_deps([
        ":otlp_file_exporter",
        "//api",
        "@googletest//:gtest_main",
        "@nlohmann_json//:json",
        "@opentelemetry-proto//:trace_service_grpc_cc",
        "@@grpc~//:grpc++",
    ]),
)

otel_cc_test(
    name = "otlp_file_exporter_factory_test",
    srcs = ["test/otlp_file_exporter_factory_test.cc"],
    tags = [
        "otlp",
        "otlp_file",
        "test",
    ],
    deps = dll_deps([
        ":otlp_file_exporter",
        "//api",
        "@googletest//:gtest_main",
    ]),
)

otel_cc_test(
    name = "otlp_http_log_record_exporter_test",
    srcs = ["test/otlp_http_log_record_exporter_test.cc"],
    tags = [
        "otlp",
        "otlp_http_log",
        "test",
    ],
    deps = dll_deps([
        ":otlp_http_log_record_exporter",
        "//api",
        "//test_common/src/http/client/nosend:http_client_nosend",
        "@googletest//:gtest_main",
    ]),
)

otel_cc_test(
    name = "otlp_http_log_record_exporter_factory_test",
    srcs = ["test/otlp_http_log_record_exporter_factory_test.cc"],
    tags = [
        "otlp",
        "otlp_http_log",
        "test",
    ],
    deps = dll_deps([
        ":otlp_http_log_record_exporter",
        "//api",
        "//test_common/src/http/client/nosend:http_client_nosend",
        "@googletest//:gtest_main",
    ]),
)

otel_cc_test(
    name = "otlp_file_log_record_exporter_test",
    srcs = ["test/otlp_file_log_record_exporter_test.cc"],
    tags = [
        "otlp",
        "otlp_file_log",
        "test",
    ],
    deps = dll_deps([
        ":otlp_file_log_record_exporter",
        "//api",
        "@googletest//:gtest_main",
        "@opentelemetry-proto//:logs_service_proto_cc",
        "@nlohmann_json//:json",
    ]),
)

otel_cc_test(
    name = "otlp_file_log_record_exporter_factory_test",
    srcs = ["test/otlp_file_log_record_exporter_factory_test.cc"],
    tags = [
        "otlp",
        "otlp_file_log",
        "test",
    ],
    deps = dll_deps([
        ":otlp_file_log_record_exporter",
        "//api",
        "@googletest//:gtest_main",
    ]),
)

otel_cc_test(
    name = "otlp_grpc_log_record_exporter_test",
    srcs = ["test/otlp_grpc_log_record_exporter_test.cc"],
    tags = [
        "otlp",
        "otlp_grpc_log",
        "test",
    ],
    deps = dll_deps([
        ":otlp_grpc_exporter",
        ":otlp_grpc_log_record_exporter",
        "//api",
        "//sdk/src/logs",
        "@googletest//:gtest_main",
        "@opentelemetry-proto//:logs_service_grpc_cc",
        "@opentelemetry-proto//:trace_service_grpc_cc",
        "@@grpc~//:grpc++",
    ]),
)

otel_cc_test(
    name = "otlp_grpc_log_record_exporter_factory_test",
    srcs = ["test/otlp_grpc_log_record_exporter_factory_test.cc"],
    tags = [
        "otlp",
        "otlp_grpc_log",
        "test",
    ],
    deps = dll_deps([
        ":otlp_grpc_log_record_exporter",
        "//api",
        "//sdk/src/logs",
        "@googletest//:gtest_main",
    ]),
)

otel_cc_test(
    name = "otlp_grpc_metric_exporter_test",
    srcs = ["test/otlp_grpc_metric_exporter_test.cc"],
    tags = [
        "otlp",
        "otlp_grpc_metric",
        "test",
    ],
    deps = [
        ":otlp_grpc_metric_exporter",
        "//api",
        "//sdk/src/metrics",
        "@googletest//:gtest_main",
    ],
)

otel_cc_test(
    name = "otlp_grpc_metric_exporter_factory_test",
    srcs = ["test/otlp_grpc_metric_exporter_factory_test.cc"],
    tags = [
        "otlp",
        "otlp_grpc_metric",
        "test",
    ],
    deps = dll_deps([
        ":otlp_grpc_metric_exporter",
        "//api",
        "@googletest//:gtest_main",
    ]),
)

otel_cc_test(
    name = "otlp_http_metric_exporter_test",
    srcs = ["test/otlp_http_metric_exporter_test.cc"],
    tags = [
        "otlp",
        "otlp_http_metric",
        "test",
    ],
    deps = dll_deps([
        ":otlp_http_metric_exporter",
        "//api",
        "//test_common/src/http/client/nosend:http_client_nosend",
        "@googletest//:gtest_main",
        "@opentelemetry-proto//:metrics_service_proto_cc",
        "@nlohmann_json//:json",
    ]),
)

otel_cc_test(
    name = "otlp_http_metric_exporter_factory_test",
    srcs = ["test/otlp_http_metric_exporter_factory_test.cc"],
    tags = [
        "otlp",
        "otlp_http_metric",
        "test",
    ],
    deps = dll_deps([
        ":otlp_http_metric_exporter",
        "//api",
        "//test_common/src/http/client/nosend:http_client_nosend",
        "@googletest//:gtest_main",
    ]),
)

otel_cc_test(
    name = "otlp_file_metric_exporter_test",
    srcs = ["test/otlp_file_metric_exporter_test.cc"],
    tags = [
        "otlp",
        "otlp_file_metric",
        "test",
    ],
    deps = [
        ":otlp_file_metric_exporter",
        "//api",
        "@googletest//:gtest_main",
    ],
)

otel_cc_test(
    name = "otlp_file_metric_exporter_factory_test",
    srcs = ["test/otlp_file_metric_exporter_factory_test.cc"],
    tags = [
        "otlp",
        "otlp_file_metric",
        "test",
    ],
    deps = [
        ":otlp_file_metric_exporter",
        "//api",
        "@googletest//:gtest_main",
    ],
)

otel_cc_benchmark(
    name = "otlp_grpc_exporter_benchmark",
    srcs = ["test/otlp_grpc_exporter_benchmark.cc"],
    tags = [
        "benchmark",
        "otlp",
        "otlp_grpc",
        "test",
    ],
    deps = dll_deps([
        ":otlp_grpc_exporter",
        "//examples/common/foo_library:common_foo_library",
        "@@grpc~//:grpc++",
        "@opentelemetry-proto//:trace_service_grpc_cc",
    ]),
)

otel_cc_library(
    name = "headers",
    hdrs = glob(["include/**/*.h"]),
    strip_include_prefix = "include",
    deps = [
        "//sdk:headers",
    ],
)

# This is used for otel_sdk.zip packaging
filegroup(
    name = "header_files",
    srcs = glob(["include/**/*.h"]),
)
