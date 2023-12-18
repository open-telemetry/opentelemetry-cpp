# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("//:dll_deps.bzl", "dll_deps", "avoid_dll_lock")

avoid_dll_lock()

cc_test(
    name = "provider_test",
    srcs = [
        "provider_test.cc",
    ],
    tags = [
        "api",
        "logs",
        "test",
    ],
    deps = dll_deps([
        "//api",
        "@com_google_googletest//:gtest_main",
    ]),
)

cc_test(
    name = "logger_test",
    srcs = [
        "logger_test.cc",
    ],
    tags = [
        "api",
        "logs",
        "test",
    ],
    deps = dll_deps([
        "//api",
        "@com_google_googletest//:gtest_main",
    ]),
)
