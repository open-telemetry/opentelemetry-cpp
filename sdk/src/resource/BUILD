# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("//bazel:otel_cc.bzl", "otel_cc_library")

package(default_visibility = ["//visibility:public"])

otel_cc_library(
    name = "resource",
    srcs = glob(["**/*.cc"]),
    include_prefix = "src/resource",
    deps = [
        "//api",
        "//sdk:headers",
        "//sdk/src/common:env_variables",
    ],
)
