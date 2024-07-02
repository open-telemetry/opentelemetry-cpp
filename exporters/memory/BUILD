# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("//:dll_deps.bzl", "dll_deps", "avoid_dll_lock")
load("//bazel:otel_cc.bzl", "otel_cc_library", "otel_cc_binary", "otel_cc_test", "otel_cc_import", "otel_cc_shared_library")

avoid_dll_lock()

package(default_visibility = ["//visibility:public"])

otel_cc_library(
    name = "in_memory_span_data",
    hdrs = [
        "include/opentelemetry/exporters/memory/in_memory_data.h",
        "include/opentelemetry/exporters/memory/in_memory_span_data.h",
    ],
    strip_include_prefix = "include",
    tags = ["memory"],
    deps = [
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
        "@com_google_googletest//:gtest_main",
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
        "@com_google_googletest//:gtest_main",
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
