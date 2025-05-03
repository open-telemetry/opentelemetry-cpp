# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("//:dll_deps.bzl", "dll_deps")
load("//bazel:otel_cc.bzl", "otel_cc_test")

otel_cc_test(
    name = "hex_test",
    srcs = [
        "hex_test.cc",
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
    name = "string_test",
    srcs = [
        "string_test.cc",
    ],
    tags = [
        "api",
        "test",
        "trace",
    ],
    deps = [
        "//api",
        "@googletest//:gtest_main",
    ],
)
