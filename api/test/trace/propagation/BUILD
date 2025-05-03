# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("//:dll_deps.bzl", "dll_deps")
load("//bazel:otel_cc.bzl", "otel_cc_test")

otel_cc_test(
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
        "@googletest//:gtest_main",
    ]),
)

otel_cc_test(
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
        "@googletest//:gtest_main",
    ]),
)

otel_cc_test(
    name = "jaeger_propagation_test",
    srcs = [
        "jaeger_propagation_test.cc",
        "util.h",
    ],
    tags = [
        "api",
        "jaeger",
        "test",
        "trace",
    ],
    deps = dll_deps([
        "//api",
        "@googletest//:gtest_main",
    ]),
)
