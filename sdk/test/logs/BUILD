# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("//:dll_deps.bzl", "dll_deps", "avoid_dll_lock")
load("//bazel:otel_cc.bzl", "otel_cc_library", "otel_cc_binary", "otel_cc_test", "otel_cc_import", "otel_cc_shared_library")

avoid_dll_lock()

otel_cc_test(
    name = "logger_provider_sdk_test",
    srcs = [
        "logger_provider_sdk_test.cc",
    ],
    tags = [
        "logs",
        "test",
    ],
    deps = dll_deps([
        "//api",
        "//sdk/src/logs",
        "@@googletest~//:gtest_main",
    ]),
)

otel_cc_test(
    name = "logger_sdk_test",
    srcs = [
        "logger_sdk_test.cc",
    ],
    tags = [
        "logs",
        "test",
    ],
    deps = dll_deps([
        "//sdk/src/logs",
        "//sdk/src/trace",
        "@@googletest~//:gtest_main",
    ]),
)

otel_cc_test(
    name = "simple_log_record_processor_test",
    srcs = [
        "simple_log_record_processor_test.cc",
    ],
    tags = [
        "logs",
        "test",
    ],
    deps = dll_deps([
        "//sdk/src/logs",
        "@@googletest~//:gtest_main",
    ]),
)

otel_cc_test(
    name = "log_record_test",
    srcs = [
        "log_record_test.cc",
    ],
    tags = [
        "logs",
        "test",
    ],
    deps = dll_deps([
        "//sdk/src/logs",
        "@@googletest~//:gtest_main",
    ]),
)

otel_cc_test(
    name = "batch_log_record_processor_test",
    srcs = [
        "batch_log_record_processor_test.cc",
    ],
    tags = [
        "logs",
        "test",
    ],
    deps = dll_deps([
        "//sdk/src/logs",
        "@@googletest~//:gtest_main",
    ]),
)
