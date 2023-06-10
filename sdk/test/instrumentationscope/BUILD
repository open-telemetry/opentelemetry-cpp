# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("//:dll_deps.bzl", "dll_deps")

cc_test(
    name = "instrumentationscope_test",
    srcs = [
        "instrumentationscope_test.cc",
    ],
    tags = ["test"],
    deps = dll_deps([
        "//api",
        "//sdk:headers",
        "@com_google_googletest//:gtest_main",
    ]),
)
