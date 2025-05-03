# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("//:dll_deps.bzl", "dll_deps")
load("//bazel:otel_cc.bzl", "otel_cc_test")

otel_cc_test(
    name = "timestamp_test",
    srcs = [
        "timestamp_test.cc",
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
    name = "version_test",
    srcs = [
        "version_test.cc",
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
