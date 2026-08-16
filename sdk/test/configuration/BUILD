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
        "config_test_logs.h",
        "config_test_metrics.h",
        "config_test_trace.h",
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
        "config_test_logs.h",
        "config_test_metrics.h",
        "config_test_propagators.h",
        "config_test_trace.h",
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
        "config_test_trace.h",
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
        "config_test_logs.h",
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
        "config_test_metrics.h",
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
        "//test_common:headers",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "propagator_builders_test",
    srcs = [
        "propagator_builders_test.cc",
    ],
    tags = [
        "config",
        "test",
    ],
    deps = [
        "//sdk:headers",
        "//sdk/src/configuration:configuration_core",
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
        "//sdk/src/configuration",
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
        "//sdk/src/configuration",
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
        "//sdk/src/configuration",
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
        "//sdk/src/configuration",
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
        "//sdk/src/configuration",
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
        "//sdk/src/configuration",
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
        "//sdk/src/configuration",
        "//sdk/src/logs",
        "//sdk/src/metrics",
        "//sdk/src/trace",
        "@com_google_googletest//:gtest_main",
    ],
)
