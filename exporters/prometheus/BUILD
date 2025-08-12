# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("//:dll_deps.bzl", "dll_deps")
load("//bazel:otel_cc.bzl", "otel_cc_library", "otel_cc_test", "otel_cc_test_library")

package(default_visibility = ["//visibility:public"])

otel_cc_library(
    name = "prometheus_exporter",
    srcs = [
        "src/exporter.cc",
        "src/exporter_factory.cc",
        "src/exporter_options.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/prometheus/exporter.h",
        "include/opentelemetry/exporters/prometheus/exporter_factory.h",
        "include/opentelemetry/exporters/prometheus/exporter_options.h",
    ],
    strip_include_prefix = "include",
    tags = ["prometheus"],
    deps = [
        ":prometheus_collector",
        ":prometheus_exporter_utils",
        "//api",
        "//sdk:headers",
        "@prometheus-cpp//core",
        "@prometheus-cpp//pull",
    ],
)

otel_cc_library(
    name = "prometheus_exporter_utils",
    srcs = [
        "src/exporter_utils.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/prometheus/exporter_utils.h",
    ],
    strip_include_prefix = "include",
    tags = ["prometheus"],
    deps = [
        "//api",
        "//sdk:headers",
        "@prometheus-cpp//core",
        "@prometheus-cpp//pull",
    ],
)

otel_cc_library(
    name = "prometheus_collector",
    srcs = [
        "src/collector.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/prometheus/collector.h",
    ],
    strip_include_prefix = "include",
    tags = ["prometheus"],
    deps = [
        ":prometheus_exporter_utils",
        "//api",
        "//sdk:headers",
        "@prometheus-cpp//core",
        "@prometheus-cpp//pull",
    ],
)

otel_cc_test_library(
    name = "prometheus_test_helper",
    hdrs = [
        "test/prometheus_test_helper.h",
    ],
    tags = ["prometheus"],
    deps = dll_deps([
        "//api",
        "//sdk:headers",
        "//sdk/src/trace",
    ]),
)

otel_cc_test(
    name = "prometheus_exporter_test",
    srcs = [
        "test/exporter_test.cc",
    ],
    tags = [
        "manual",  # malkia: this has recently became broken, civetweb can't allocate port on localhost:9464
        "prometheus",
        "test",
    ],
    deps = dll_deps([
        ":prometheus_exporter",
        ":prometheus_test_helper",
        "//sdk/src/metrics",
        "@googletest//:gtest_main",
        "@prometheus-cpp//core",
        "@prometheus-cpp//pull",
    ]),
)

otel_cc_test(
    name = "prometheus_collector_test",
    srcs = [
        "test/collector_test.cc",
    ],
    tags = [
        "prometheus",
        "test",
    ],
    deps = dll_deps([
        ":prometheus_collector",
        ":prometheus_test_helper",
        "//sdk/src/metrics",
        "@googletest//:gtest_main",
        "@prometheus-cpp//core",
        "@prometheus-cpp//pull",
    ]),
)

otel_cc_test(
    name = "prometheus_exporter_utils_test",
    srcs = [
        "test/exporter_utils_test.cc",
    ],
    tags = [
        "prometheus",
        "test",
    ],
    deps = dll_deps([
        ":prometheus_exporter_utils",
        ":prometheus_test_helper",
        "@googletest//:gtest_main",
        "@prometheus-cpp//core",
        "@prometheus-cpp//pull",
    ]),
)

otel_cc_library(
    name = "prometheus_push_exporter",
    srcs = [
        "src/push_exporter.cc",
        "src/push_exporter_factory.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/prometheus/push_exporter.h",
        "include/opentelemetry/exporters/prometheus/push_exporter_factory.h",
        "include/opentelemetry/exporters/prometheus/push_exporter_options.h",
    ],
    strip_include_prefix = "include",
    tags = ["prometheus"],
    deps = [
        "//api",
        "//exporters/prometheus:prometheus_collector",
        "//exporters/prometheus:prometheus_exporter_utils",
        "//sdk:headers",
        "//sdk/src/metrics",
        "@prometheus-cpp//core",
        "@prometheus-cpp//push",
    ],
)

otel_cc_test_library(
    name = "prometheus_push_test_helper",
    hdrs = [
        "test/prometheus_push_test_helper.h",
    ],
    tags = ["prometheus"],
    deps = dll_deps([
        "//api",
        "//sdk:headers",
        "//sdk/src/trace",
    ]),
)

otel_cc_test(
    name = "prometheus_push_exporter_test",
    srcs = [
        "test/push_exporter_test.cc",
    ],
    flaky = True,
    tags = [
        "prometheus",
        "test",
    ],
    deps = dll_deps([
        ":prometheus_push_exporter",
        ":prometheus_push_test_helper",
        "@googletest//:gtest_main",
        "@prometheus-cpp//push",
        "//api",
        "//exporters/prometheus:prometheus_collector",
        "//exporters/prometheus:prometheus_exporter_utils",
        "//sdk:headers",
    ]),
)

filegroup(
    name = "header_files",
    srcs = glob(["include/**/*.h"]),
)

otel_cc_library(
    name = "headers",
    hdrs = [":header_files"],
    strip_include_prefix = "include",
    deps = [
        "//sdk:headers",
    ],
)
