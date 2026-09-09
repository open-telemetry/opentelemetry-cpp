# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("@rules_cc//cc:cc_test.bzl", "cc_test")

cc_test(
    name = "registry_test",
    srcs = [
        "registry_test.cc",
    ],
    tags = [
        "config",
        "test",
    ],
    deps = [
        "//sdk:headers",
        "//sdk/src/configuration:configuration_core",
        "//sdk/src/logs",
        "//sdk/src/metrics",
        "//sdk/src/resource",
        "//sdk/src/trace",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "sdk_builder_test",
    srcs = [
        "config_test_common.h",
        "sdk_builder_test.cc",
    ],
    tags = [
        "config",
        "test",
    ],
    deps = [
        "//sdk:headers",
        "//sdk/src/configuration:configuration_registry_factory",
        "//sdk/src/logs",
        "//sdk/src/metrics",
        "//sdk/src/resource",
        "//sdk/src/trace",
        "//test_common:headers",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "configured_sdk_test",
    srcs = [
        "config_test_common.h",
        "configured_sdk_test.cc",
    ],
    tags = [
        "config",
        "test",
    ],
    deps = [
        "//sdk:headers",
        "//sdk/src/configuration:configuration_registry_factory",
        "//sdk/src/logs",
        "//sdk/src/metrics",
        "//sdk/src/resource",
        "//sdk/src/trace",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "programmatic_configuration_test",
    srcs = [
        "config_test_common.h",
        "programmatic_configuration_test.cc",
    ],
    tags = [
        "config",
        "test",
    ],
    deps = [
        "//sdk:headers",
        "//sdk/src/configuration:configuration_registry_factory",
        "//sdk/src/logs",
        "//sdk/src/metrics",
        "//sdk/src/resource",
        "//sdk/src/trace",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "trace_builders_test",
    srcs = [
        "trace_builders_test.cc",
    ],
    tags = [
        "config",
        "test",
    ],
    deps = [
        "//sdk:headers",
        "//sdk/src/configuration:configuration_trace_builders",
        "//sdk/src/trace",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "logs_builders_test",
    srcs = [
        "logs_builders_test.cc",
    ],
    tags = [
        "config",
        "test",
    ],
    deps = [
        "//sdk:headers",
        "//sdk/src/configuration:configuration_logs_builders",
        "//sdk/src/logs",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "metrics_builders_test",
    srcs = [
        "metrics_builders_test.cc",
    ],
    tags = [
        "config",
        "test",
    ],
    deps = [
        "//sdk:headers",
        "//sdk/src/configuration:configuration_metrics_builders",
        "//sdk/src/metrics",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "yaml_logs_test",
    srcs = [
        "yaml_logs_test.cc",
    ],
    tags = [
        "test",
        "yaml",
    ],
    deps = [
        "//api",
        "//sdk:headers",
        "//sdk/src/configuration:configuration_yaml",
        "//sdk/src/logs",
        "//sdk/src/metrics",
        "//sdk/src/trace",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "yaml_metrics_test",
    srcs = [
        "yaml_metrics_test.cc",
    ],
    tags = [
        "test",
        "yaml",
    ],
    deps = [
        "//api",
        "//sdk:headers",
        "//sdk/src/configuration:configuration_yaml",
        "//sdk/src/logs",
        "//sdk/src/metrics",
        "//sdk/src/trace",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "yaml_propagator_test",
    srcs = [
        "yaml_propagator_test.cc",
    ],
    tags = [
        "test",
        "yaml",
    ],
    deps = [
        "//api",
        "//sdk:headers",
        "//sdk/src/configuration:configuration_yaml",
        "//sdk/src/logs",
        "//sdk/src/metrics",
        "//sdk/src/trace",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "yaml_resource_test",
    srcs = [
        "yaml_resource_test.cc",
    ],
    tags = [
        "test",
        "yaml",
    ],
    deps = [
        "//api",
        "//sdk:headers",
        "//sdk/src/configuration:configuration_yaml",
        "//sdk/src/logs",
        "//sdk/src/metrics",
        "//sdk/src/trace",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "yaml_test",
    srcs = [
        "yaml_test.cc",
    ],
    tags = [
        "test",
        "yaml",
    ],
    deps = [
        "//api",
        "//sdk:headers",
        "//sdk/src/configuration:configuration_yaml",
        "//sdk/src/logs",
        "//sdk/src/metrics",
        "//sdk/src/trace",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "yaml_trace_test",
    srcs = [
        "yaml_trace_test.cc",
    ],
    tags = [
        "test",
        "yaml",
    ],
    deps = [
        "//api",
        "//sdk:headers",
        "//sdk/src/configuration:configuration_yaml",
        "//sdk/src/logs",
        "//sdk/src/metrics",
        "//sdk/src/trace",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "yaml_distribution_test",
    srcs = [
        "yaml_distribution_test.cc",
    ],
    tags = [
        "test",
        "yaml",
    ],
    deps = [
        "//api",
        "//sdk:headers",
        "//sdk/src/configuration:configuration_yaml",
        "//sdk/src/logs",
        "//sdk/src/metrics",
        "//sdk/src/trace",
        "@com_google_googletest//:gtest_main",
    ],
)
