# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("//bazel:otel_cc_library.bzl", "otel_cc_library")

package(default_visibility = ["//visibility:public"])

otel_cc_library(
    name = "metrics",
    srcs = glob(["**/*.cc"]),
    hdrs = glob(["**/*.h"], allow_empty = True ),
    include_prefix = "src/metrics",
    deps = [
        "//api",
        "//sdk:headers",
        "//sdk/src/common:global_log_handler",
        "//sdk/src/common:random",
        "//sdk/src/resource",
    ],
)
