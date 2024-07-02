# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("//bazel:otel_cc_benchmark.bzl", "otel_cc_benchmark")
load("//:dll_deps.bzl", "dll_deps", "avoid_dll_lock")
load("//bazel:otel_cc.bzl", "otel_cc_library", "otel_cc_binary", "otel_cc_test", "otel_cc_import", "otel_cc_shared_library")

avoid_dll_lock()

otel_cc_test(
    name = "random_test",
    srcs = [
        "random_test.cc",
    ],
    tags = ["test"],
    deps = dll_deps([
        "//sdk/src/common:random",
        "@@googletest~//:gtest_main",
    ]),
)

otel_cc_test(
    name = "base64_test",
    srcs = [
        "base64_test.cc",
    ],
    tags = ["test"],
    deps = dll_deps([
        "//sdk/src/common:base64",
        "@@googletest~//:gtest_main",
    ]),
)

otel_cc_test(
    name = "fast_random_number_generator_test",
    srcs = [
        "fast_random_number_generator_test.cc",
    ],
    tags = ["test"],
    deps = dll_deps([
        "//sdk/src/common:random",
        "@@googletest~//:gtest_main",
    ]),
)

otel_cc_benchmark(
    name = "base64_benchmark",
    srcs = ["base64_benchmark.cc"],
    tags = [
        "benchmark",
        "test",
    ],
    deps = dll_deps(["//sdk/src/common:base64"]),
)

otel_cc_benchmark(
    name = "random_benchmark",
    srcs = ["random_benchmark.cc"],
    tags = [
        "benchmark",
        "test",
    ],
    deps = dll_deps(["//sdk/src/common:random"]),
)

otel_cc_test(
    name = "atomic_unique_ptr_test",
    srcs = [
        "atomic_unique_ptr_test.cc",
    ],
    tags = ["test"],
    deps = dll_deps([
        "//api",
        "//sdk:headers",
        "@@googletest~//:gtest_main",
    ]),
)

otel_cc_test(
    name = "circular_buffer_range_test",
    srcs = [
        "circular_buffer_range_test.cc",
    ],
    tags = ["test"],
    deps = dll_deps([
        "//api",
        "//sdk:headers",
        "@@googletest~//:gtest_main",
    ]),
)

otel_cc_test(
    name = "random_fork_test",
    srcs = [
        "random_fork_test.cc",
    ],
    tags = ["test"],
    deps = dll_deps([
        "//sdk/src/common:random",
    ]),
)

otel_cc_library(
    name = "baseline_circular_buffer",
    testonly = 1,
    hdrs = [
        "baseline_circular_buffer.h",
    ],
    include_prefix = "test/common",
    deps = dll_deps([
        "//api",
    ]),
)

otel_cc_benchmark(
    name = "circular_buffer_benchmark",
    srcs = ["circular_buffer_benchmark.cc"],
    tags = [
        "benchmark",
        "test",
    ],
    deps = dll_deps([
        ":baseline_circular_buffer",
        "//sdk:headers",
    ]),
)

otel_cc_test(
    name = "empty_attributes_test",
    srcs = [
        "empty_attributes_test.cc",
    ],
    tags = ["test"],
    deps = dll_deps([
        "//api",
        "//sdk:headers",
        "@@googletest~//:gtest_main",
    ]),
)

otel_cc_test(
    name = "attribute_utils_test",
    srcs = [
        "attribute_utils_test.cc",
    ],
    tags = ["test"],
    deps = dll_deps([
        "//api",
        "//sdk:headers",
        "@@googletest~//:gtest_main",
    ]),
)

otel_cc_test(
    name = "global_log_handle_test",
    srcs = [
        "global_log_handle_test.cc",
    ],
    tags = ["test"],
    deps = dll_deps([
        "//api",
        "//sdk:headers",
        "//sdk/src/common:global_log_handler",
        "@@googletest~//:gtest_main",
    ]),
)

otel_cc_test(
    name = "attributemap_hash_test",
    srcs = [
        "attributemap_hash_test.cc",
    ],
    tags = ["test"],
    deps = dll_deps([
        "//api",
        "//sdk:headers",
        "@@googletest~//:gtest_main",
    ]),
)

otel_cc_benchmark(
    name = "attributemap_hash_benchmark",
    srcs = ["attributemap_hash_benchmark.cc"],
    tags = [
        "benchmark",
        "test",
    ],
    deps = dll_deps([
        "//api",
        "//sdk:headers",
    ]),
)

otel_cc_test(
    name = "env_var_test",
    srcs = [
        "env_var_test.cc",
    ],
    tags = ["test"],
    deps = dll_deps([
        "//sdk/src/common:env_variables",
        "@@googletest~//:gtest_main",
    ]),
)
