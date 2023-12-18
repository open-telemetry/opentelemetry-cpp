# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("//bazel:otel_cc_benchmark.bzl", "otel_cc_benchmark")
load("//:dll_deps.bzl", "dll_deps", "avoid_dll_lock")

avoid_dll_lock()

cc_test(
    name = "http_text_format_test",
    srcs = [
        "http_text_format_test.cc",
        "util.h",
    ],
    tags = [
        "api",
        "test",
        "trace",
    ],
    deps = dll_deps([
        "//api",
        "@com_google_googletest//:gtest_main",
    ]),
)

cc_test(
    name = "b3_propagation_test",
    srcs = [
        "b3_propagation_test.cc",
        "util.h",
    ],
    tags = [
        "api",
        "test",
        "trace",
    ],
    deps = dll_deps([
        "//api",
        "@com_google_googletest//:gtest_main",
    ]),
)

cc_test(
    name = "jaeger_propagation_test",
    srcs = [
        "jaeger_propagation_test.cc",
        "util.h",
    ],
    tags = [
        "api",
        "test",
        "trace",
        "jaeger",
    ],
    deps = dll_deps([
        "//api",
        "@com_google_googletest//:gtest_main",
    ]),
)
