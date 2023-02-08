# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("//:dll_deps.bzl", "dll_deps")

cc_test(
    name = "threadsafe_span_data_tests",
    srcs = [
        "threadsafe_span_data_test.cc",
    ],
    tags = ["test"],
    deps = dll_deps([
        "//ext/src/zpages",
        "//sdk/src/trace",
    ]) + [
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "tracez_data_aggregator_tests",
    srcs = [
        "tracez_data_aggregator_test.cc",
    ],
    tags = ["test"],
    deps = dll_deps([
        "//ext/src/zpages",
        "//sdk/src/trace",
    ]) + [
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "tracez_processor_tests",
    srcs = [
        "tracez_processor_test.cc",
    ],
    tags = ["test"],
    deps = dll_deps([
        "//ext/src/zpages",
        "//sdk/src/trace",
    ]) + [
        "@com_google_googletest//:gtest_main",
    ],
)
