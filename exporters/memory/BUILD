# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("//:dll_deps.bzl", "dll_deps")
load("//bazel:otel_cc.bzl", "otel_cc_library", "otel_cc_test")

package(default_visibility = ["//visibility:public"])

otel_cc_library(
    name = "in_memory_metric_data",
    srcs = [
        "src/in_memory_metric_data.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/memory/in_memory_metric_data.h",
    ],
    strip_include_prefix = "include",
    tags = [
        "memory",
        "test",
    ],
    deps = [
        ":in_memory_data",
        "//sdk/src/metrics",
    ],
)

otel_cc_test(
    name = "in_memory_metric_data_test",
    srcs = ["test/in_memory_metric_data_test.cc"],
    tags = [
        "memory",
        "test",
    ],
    deps = dll_deps([
        ":in_memory_metric_data",
        "@googletest//:gtest_main",
    ]),
)

otel_cc_library(
    name = "in_memory_metric_exporter_factory",
    srcs = [
        "src/in_memory_metric_exporter_factory.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/memory/in_memory_metric_exporter_factory.h",
    ],
    strip_include_prefix = "include",
    tags = [
        "memory",
        "test",
    ],
    deps = [
        ":in_memory_metric_data",
        "//sdk/src/metrics",
    ],
)

otel_cc_test(
    name = "in_memory_metric_exporter_test",
    srcs = ["test/in_memory_metric_exporter_test.cc"],
    tags = [
        "memory",
        "test",
    ],
    deps = dll_deps([
        ":in_memory_metric_exporter_factory",
        "@googletest//:gtest_main",
    ]),
)

otel_cc_library(
    name = "in_memory_data",
    hdrs = [
        "include/opentelemetry/exporters/memory/in_memory_data.h",
    ],
    strip_include_prefix = "include",
    tags = ["memory"],
    deps = [
        "//sdk:headers",
    ],
)

otel_cc_library(
    name = "in_memory_span_data",
    hdrs = [
        "include/opentelemetry/exporters/memory/in_memory_span_data.h",
    ],
    strip_include_prefix = "include",
    tags = ["memory"],
    deps = [
        ":in_memory_data",
        "//api",
        "//sdk/src/resource",
        "//sdk/src/trace",
    ],
)

otel_cc_test(
    name = "in_memory_span_data_test",
    srcs = ["test/in_memory_span_data_test.cc"],
    tags = [
        "memory",
        "test",
    ],
    deps = dll_deps([
        ":in_memory_span_data",
        ":in_memory_data",
        "@googletest//:gtest_main",
    ]),
)

otel_cc_library(
    name = "in_memory_span_exporter",
    srcs = [
        "src/in_memory_span_exporter_factory.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/memory/in_memory_span_exporter.h",
        "include/opentelemetry/exporters/memory/in_memory_span_exporter_factory.h",
    ],
    strip_include_prefix = "include",
    tags = [
        "memory",
        "test",
    ],
    deps = [
        ":in_memory_span_data",
        ":in_memory_data",
        "//sdk/src/trace",
    ],
)

otel_cc_test(
    name = "in_memory_span_exporter_test",
    srcs = ["test/in_memory_span_exporter_test.cc"],
    tags = [
        "memory",
        "test",
    ],
    deps = dll_deps([
        ":in_memory_span_exporter",
        "@googletest//:gtest_main",
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
