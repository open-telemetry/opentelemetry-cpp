# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("//:dll_deps.bzl", "dll_deps")

cc_test(
    name = "resource_test",
    srcs = [
        "resource_test.cc",
    ],
    tags = ["test"],
    deps = dll_deps([
        "//api",
        "//sdk/src/resource",
        "@com_google_googletest//:gtest_main",
    ]),
)
