# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("//:dll_deps.bzl", "dll_deps")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "prometheus_exporter",
    srcs = [
        "src/exporter.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/prometheus/exporter.h",
    ],
    strip_include_prefix = "include",
    tags = ["prometheus"],
    deps = [
        ":prometheus_collector",
        ":prometheus_exporter_utils",
        "//api",
        "//sdk:headers",
        "@com_github_jupp0r_prometheus_cpp//core",
        "@com_github_jupp0r_prometheus_cpp//pull",
    ],
)

cc_library(
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
        "@com_github_jupp0r_prometheus_cpp//core",
        "@com_github_jupp0r_prometheus_cpp//pull",
    ],
)

cc_library(
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
        "@com_github_jupp0r_prometheus_cpp//core",
        "@com_github_jupp0r_prometheus_cpp//pull",
    ],
)

cc_library(
    name = "headers",
    hdrs = glob(["include/**/*.h"]),
    strip_include_prefix = "include",
    deps = [
        "//sdk:headers",
    ],
)

# This is used for otel_sdk.zip packaging
filegroup(
    name="header_files",
    srcs=glob(["include/**/*.h"]),
)
