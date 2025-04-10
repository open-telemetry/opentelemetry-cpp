# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("//:dll_deps.bzl", "dll_deps")
load("//bazel:otel_cc.bzl", "otel_cc_library", "otel_cc_test")
load("//bazel:otel_cc_benchmark.bzl", "otel_cc_benchmark")

package(default_visibility = ["//visibility:public"])

otel_cc_library(
    name = "etw_exporter",
    local_defines = [
        "HAVE_MSGPACK",
    ],
    tags = ["etw"],
    deps = [
        "headers",
        "//:windows_only",
        "//api",
        "//sdk/src/trace",
        "@nlohmann_json//:json",
    ],
)

otel_cc_test(
    name = "etw_provider_test",
    srcs = ["test/etw_provider_test.cc"],
    local_defines = [
        "HAVE_MSGPACK",
    ],
    tags = [
        "etw",
        "test",
    ],
    deps = dll_deps([
        ":etw_exporter",
        "@googletest//:gtest_main",
        "@nlohmann_json//:json",
        "//:windows_only",
    ]),
)

otel_cc_test(
    name = "etw_tracer_test",
    srcs = ["test/etw_tracer_test.cc"],
    local_defines = [
        "HAVE_MSGPACK",
    ],
    tags = [
        "etw",
        "test",
    ],
    deps = dll_deps([
        ":etw_exporter",
        "@googletest//:gtest_main",
        "@nlohmann_json//:json",
        "//:windows_only",
    ]),
)

otel_cc_test(
    name = "etw_logger_test",
    srcs = ["test/etw_logger_test.cc"],
    local_defines = [
        "HAVE_MSGPACK",
    ],
    tags = [
        "etw",
        "test",
    ],
    deps = dll_deps([
        ":etw_exporter",
        "@googletest//:gtest_main",
        "@nlohmann_json//:json",
        "//:windows_only",
    ]),
)

otel_cc_benchmark(
    name = "etw_perf_test",
    srcs = ["test/etw_perf_test.cc"],
    tags = [
        "benchmark",
        "etw",
        "test",
    ],
    deps = dll_deps([
        ":etw_exporter",
        "@googletest//:gtest_main",
        "//:windows_only",
    ]),
)

otel_cc_library(
    name = "headers",
    hdrs = glob(["include/**/*.h"]),
    strip_include_prefix = "include",
    deps = [
        "//:windows_only",
        "//sdk:headers",
    ],
)

# This is used for otel_sdk.zip packaging
filegroup(
    name = "header_files",
    srcs = glob(["include/**/*.h"]),
)
