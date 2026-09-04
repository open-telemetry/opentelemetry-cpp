# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("@rules_cc//cc:cc_test.bzl", "cc_test")
load("//bazel:otel_cc_benchmark.bzl", "otel_cc_benchmark")

otel_cc_benchmark(
    name = "fixed_size_exemplar_reservoir_benchmark",
    srcs = [
        "fixed_size_exemplar_reservoir_benchmark.cc",
    ],
    defines = ["ENABLE_METRICS_EXEMPLAR_PREVIEW"],
    tags = [
        "benchmark",
        "metrics",
        "test",
    ],
    deps = [
        "//api",
        "//sdk:headers",
        "//sdk/src/common:random",
    ],
)

cc_test(
    name = "no_exemplar_reservoir_test",
    srcs = [
        "no_exemplar_reservoir_test.cc",
    ],
    tags = [
        "metrics",
        "test",
    ],
    deps = [
        "//api",
        "//sdk:headers",
        "//sdk/src/metrics",
        "@com_google_googletest//:gtest_main",
    ],
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
    deps = [
        "//api",
        "//sdk:headers",
        "//sdk/src/metrics",
        "@com_google_googletest//:gtest_main",
    ],
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
    deps = [
        "//api",
        "//sdk:headers",
        "//sdk/src/metrics",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "filter_predicate_test",
    srcs = [
        "filter_predicate_test.cc",
    ],
    tags = [
        "metrics",
        "test",
    ],
    deps = [
        "//api",
        "//sdk:headers",
        "//sdk/src/metrics",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "fixed_size_exemplar_reservoir_test",
    srcs = [
        "fixed_size_exemplar_reservoir_test.cc",
    ],
    tags = [
        "metrics",
        "test",
    ],
    deps = [
        "//api",
        "//sdk:headers",
        "//sdk/src/metrics",
        "@com_google_googletest//:gtest_main",
    ],
)
