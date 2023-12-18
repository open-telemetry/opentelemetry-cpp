# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("//:dll_deps.bzl", "dll_deps", "avoid_dll_lock")

avoid_dll_lock()

cc_test(
    name = "dynamic_load_test",
    srcs = [
        "dynamic_load_test.cc",
    ],
    linkopts = [
        "-ldl",
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
