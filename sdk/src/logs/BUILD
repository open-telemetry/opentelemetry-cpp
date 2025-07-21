# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("//bazel:otel_cc.bzl", "otel_cc_library")

package(default_visibility = ["//visibility:public"])

otel_cc_library(
    name = "logs",
    srcs = glob(["**/*.cc"]),
    include_prefix = "src/logs",
    deps = [
        "//api",
        "//sdk:headers",
        "//sdk/src/common:disabled",
        "//sdk/src/common:global_log_handler",
        "//sdk/src/resource",
    ],
)
