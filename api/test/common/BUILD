# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("//bazel:otel_cc_benchmark.bzl", "otel_cc_benchmark")
load("//:dll_deps.bzl", "dll_deps", "avoid_dll_lock")
load("//bazel:otel_cc.bzl", "otel_cc_library", "otel_cc_binary", "otel_cc_test", "otel_cc_import", "otel_cc_shared_library")

avoid_dll_lock()

otel_cc_benchmark(
    name = "spinlock_benchmark",
    srcs = ["spinlock_benchmark.cc"],
    tags = [
        "api",
        "benchmark",
        "test",
    ],
    deps = dll_deps(["//api"]),
)

otel_cc_test(
    name = "kv_properties_test",
    srcs = [
        "kv_properties_test.cc",
    ],
    tags = [
        "api",
        "test",
    ],
    deps = dll_deps([
        "//api",
        "@googletest//:gtest_main",
    ]),
)

otel_cc_test(
    name = "string_util_test",
    srcs = [
        "string_util_test.cc",
    ],
    tags = [
        "api",
        "test",
    ],
    deps = dll_deps([
        "//api",
        "@googletest//:gtest_main",
    ]),
)
