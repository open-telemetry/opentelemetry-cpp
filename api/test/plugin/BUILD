# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("//:dll_deps.bzl", "dll_deps")
load("//bazel:otel_cc.bzl", "otel_cc_test")

otel_cc_test(
    name = "dynamic_load_test",
    srcs = [
        "dynamic_load_test.cc",
    ],
    linkopts = select({
        "//bazel:windows": [],
        "//conditions:default": ["-ldl"],
    }),
    tags = [
        "api",
        "test",
    ],
    deps = dll_deps([
        "//api",
        "@googletest//:gtest_main",
    ]),
)
