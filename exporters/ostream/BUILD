# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("//:dll_deps.bzl", "dll_deps", "avoid_dll_lock")
load("//bazel:otel_cc.bzl", "otel_cc_library", "otel_cc_binary", "otel_cc_test", "otel_cc_import", "otel_cc_shared_library")

package(default_visibility = ["//visibility:public"])

avoid_dll_lock()

otel_cc_library(
    name = "ostream_log_record_exporter",
    srcs = [
        "src/log_record_exporter.cc",
        "src/log_record_exporter_factory.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/ostream/common_utils.h",
        "include/opentelemetry/exporters/ostream/log_record_exporter.h",
        "include/opentelemetry/exporters/ostream/log_record_exporter_factory.h",
    ],
    strip_include_prefix = "include",
    tags = ["ostream"],
    deps = [
        "//sdk/src/logs",
    ],
)

otel_cc_test(
    name = "ostream_log_test",
    srcs = ["test/ostream_log_test.cc"],
    tags = [
        "ostream",
        "test",
    ],
    deps = dll_deps([
        ":ostream_log_record_exporter",
        "@googletest//:gtest_main",
    ]),
)

otel_cc_library(
    name = "ostream_metric_exporter",
    srcs = [
        "src/metric_exporter.cc",
        "src/metric_exporter_factory.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/ostream/common_utils.h",
        "include/opentelemetry/exporters/ostream/metric_exporter.h",
        "include/opentelemetry/exporters/ostream/metric_exporter_factory.h",
    ],
    strip_include_prefix = "include",
    tags = [
        "metrics",
        "ostream",
    ],
    deps = [
        "//sdk/src/metrics",
    ],
)

otel_cc_test(
    name = "ostream_metric_test",
    srcs = ["test/ostream_metric_test.cc"],
    tags = [
        "ostream",
        "test",
    ],
    deps = dll_deps([
        ":ostream_metric_exporter",
        "@googletest//:gtest_main",
    ]),
)

otel_cc_library(
    name = "ostream_span_exporter",
    srcs = [
        "src/span_exporter.cc",
        "src/span_exporter_factory.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/ostream/common_utils.h",
        "include/opentelemetry/exporters/ostream/span_exporter.h",
        "include/opentelemetry/exporters/ostream/span_exporter_factory.h",
    ],
    strip_include_prefix = "include",
    tags = ["ostream"],
    deps = [
        "//sdk/src/trace",
    ],
)

otel_cc_library(
    name = "ostream_capture",
    testonly = 1,
    hdrs = [
        "test/ostream_capture.h",
    ],
    tags = ["ostream"],
    deps = dll_deps([
        "//api",
    ]),
)

otel_cc_test(
    name = "ostream_span_test",
    srcs = ["test/ostream_span_test.cc"],
    tags = [
        "ostream",
        "test",
    ],
    deps = dll_deps([
        ":ostream_capture",
        ":ostream_span_exporter",
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
    name="header_files",
    srcs=glob(["include/**/*.h"]),
)
