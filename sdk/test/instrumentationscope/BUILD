# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("@rules_cc//cc:cc_test.bzl", "cc_test")

cc_test(
    name = "instrumentationscope_test",
    srcs = [
        "instrumentationscope_test.cc",
    ],
    tags = ["test"],
    deps = [
        "//api",
        "//sdk:headers",
        "//sdk/src/common:attribute_validity",
        "@com_google_googletest//:gtest_main",
    ],
)
