# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("//bazel:otel_cc.bzl", "otel_cc_library")

package(default_visibility = ["//visibility:public"])

otel_cc_library(
    name = "version",
    srcs = [
        "version.cc",
        "version_api.cc",
    ],
    deps = [
        "//api",
        "//sdk:headers",
    ],
)
