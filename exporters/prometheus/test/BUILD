# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("//:dll_deps.bzl", "dll_deps")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "prometheus_test_helper",
    testonly = True,
    hdrs = [
        "prometheus_test_helper.h",
    ],
    tags = ["prometheus"],
    deps = [
        "//api",
        "//sdk:headers",
        "//sdk/src/trace",
    ],
)

cc_test(
    name = "prometheus_exporter_test",
    srcs = [
        "exporter_test.cc",
    ],
    tags = [
        "prometheus",
        "test",
    ],
    deps = dll_deps([
        "//exporters/prometheus:prometheus_exporter",
        ":prometheus_test_helper",
        "//sdk/src/metrics",
        "@com_google_googletest//:gtest_main",
        "@com_github_jupp0r_prometheus_cpp//core",
        "@com_github_jupp0r_prometheus_cpp//pull",
    ]),
)

cc_test(
    name = "prometheus_collector_test",
    srcs = [
        "collector_test.cc",
    ],
    tags = [
        "prometheus",
        "test",
    ],
    deps = dll_deps([
        "//exporters/prometheus:prometheus_collector",
        ":prometheus_test_helper",
        "//sdk/src/metrics",
        "@com_google_googletest//:gtest_main",
        "@com_github_jupp0r_prometheus_cpp//core",
        "@com_github_jupp0r_prometheus_cpp//pull",
    ]),
)

cc_test(
    name = "prometheus_exporter_utils_test",
    srcs = [
        "exporter_utils_test.cc",
    ],
    tags = [
        "prometheus",
        "test",
    ],
    deps = dll_deps([
        "//exporters/prometheus:prometheus_exporter_utils",
        ":prometheus_test_helper",
        "@com_google_googletest//:gtest_main",
        "@com_github_jupp0r_prometheus_cpp//core",
        "@com_github_jupp0r_prometheus_cpp//pull",
    ]),
)
