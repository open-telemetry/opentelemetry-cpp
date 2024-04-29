# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0
load("//:dll_deps.bzl", "dll_deps", "avoid_dll_lock")

avoid_dll_lock()

cc_test(
    name = "no_exemplar_reservoir_test",
    srcs = [
        "no_exemplar_reservoir_test.cc",
    ],
    tags = [
        "metrics",
        "test",
    ],
    deps = dll_deps([
        "//api",
        "//sdk:headers",
        "//sdk/src/metrics",
        "@com_google_googletest//:gtest_main",
    ]),
)

cc_test(
    name = "reservoir_cell_test",
    srcs = [
        "reservoir_cell_test.cc",
    ],
    tags = [
        "metrics",
        "test",
    ],
    deps = dll_deps([
        "//api",
        "//sdk:headers",
        "//sdk/src/metrics",
        "@com_google_googletest//:gtest_main",
    ]),
)

cc_test(
    name = "aligned_histogram_bucket_exemplar_reservoir_test",
    srcs = [
        "aligned_histogram_bucket_exemplar_reservoir_test.cc",
    ],
    tags = [
        "metrics",
        "test",
    ],
    deps = dll_deps([
        "//api",
        "//sdk:headers",
        "//sdk/src/metrics",
        "@com_google_googletest//:gtest_main",
    ]),
)
