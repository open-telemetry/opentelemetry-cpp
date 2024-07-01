# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("//bazel:otel_cc_benchmark.bzl", "otel_cc_benchmark")
load("//:dll_deps.bzl", "dll_deps", "avoid_dll_lock")
load("//bazel:otel_cc_library.bzl", "otel_cc_library")

avoid_dll_lock()

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "etw_exporter",
    hdrs = glob([
        "include/opentelemetry/exporters/etw/*.h",
    ]),
    includes = ["include"],
    local_defines = [
        "HAVE_MSGPACK",
    ],
    tags = ["etw"],
    deps = [
        "//api",
        "//sdk/src/trace",
        "@github_nlohmann_json//:json",
        "//:windows_only",
    ],
)

cc_test(
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
        "@com_google_googletest//:gtest_main",
        "@github_nlohmann_json//:json",
        "//:windows_only",
    ]),
)

cc_test(
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
        "@com_google_googletest//:gtest_main",
        "@github_nlohmann_json//:json",
        "//:windows_only",
    ]),
)

cc_test(
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
        "@com_google_googletest//:gtest_main",
        "@github_nlohmann_json//:json",
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
        "@com_google_googletest//:gtest_main",
        "//:windows_only",
    ]),
)

cc_library(
    name = "headers",
    hdrs = glob(["include/**/*.h"]),
    strip_include_prefix = "include",
    deps = [
        "//sdk:headers",
        "//:windows_only",
    ],
)

# This is used for otel_sdk.zip packaging
filegroup(
    name="header_files",
    srcs=glob(["include/**/*.h"]),
)
