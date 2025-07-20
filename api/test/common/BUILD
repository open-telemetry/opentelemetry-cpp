# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("//:dll_deps.bzl", "dll_deps")
load("//bazel:otel_cc.bzl", "otel_cc_test")
load("//bazel:otel_cc_benchmark.bzl", "otel_cc_benchmark")

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
