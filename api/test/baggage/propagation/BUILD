# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("//bazel:otel_cc_benchmark.bzl", "otel_cc_benchmark")
load("//:dll_deps.bzl", "dll_deps", "avoid_dll_lock")

avoid_dll_lock()

cc_test(
    name = "baggage_propagator_test",
    srcs = [
        "baggage_propagator_test.cc",
    ],
    tags = [
        "api",
        "test",
    ],
    deps = dll_deps([
        "//api",
        "@com_google_googletest//:gtest_main",
    ]),
)
