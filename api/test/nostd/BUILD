# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("//:dll_deps.bzl", "dll_deps", "avoid_dll_lock")

avoid_dll_lock()

cc_test(
    name = "function_ref_test",
    srcs = [
        "function_ref_test.cc",
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

cc_test(
    name = "string_view_test",
    srcs = [
        "string_view_test.cc",
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

cc_test(
    name = "variant_test",
    srcs = [
        "variant_test.cc",
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

cc_test(
    name = "unique_ptr_test",
    srcs = [
        "unique_ptr_test.cc",
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

cc_test(
    name = "utility_test",
    srcs = [
        "utility_test.cc",
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

cc_test(
    name = "span_test",
    srcs = [
        "span_test.cc",
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

cc_test(
    name = "shared_ptr_test",
    srcs = [
        "shared_ptr_test.cc",
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
