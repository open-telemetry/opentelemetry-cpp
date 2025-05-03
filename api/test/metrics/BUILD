# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("//:dll_deps.bzl", "dll_deps")
load("//bazel:otel_cc.bzl", "otel_cc_test")

otel_cc_test(
    name = "noop_sync_instrument_test",
    srcs = [
        "noop_sync_instrument_test.cc",
    ],
    tags = [
        "metrics",
        "test",
    ],
    deps = dll_deps([
        "//api",
        "@googletest//:gtest_main",
    ]),
)

otel_cc_test(
    name = "meter_provider_test",
    srcs = [
        "meter_provider_test.cc",
    ],
    tags = [
        "metrics",
        "test",
    ],
    deps = dll_deps([
        "//api",
        "@googletest//:gtest_main",
    ]),
)
