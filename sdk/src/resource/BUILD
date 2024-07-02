# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("//bazel:otel_cc.bzl", "otel_cc_library", "otel_cc_binary", "otel_cc_test", "otel_cc_import", "otel_cc_shared_library")

package(default_visibility = ["//visibility:public"])

otel_cc_library(
    name = "resource",
    srcs = glob(["**/*.cc"]),
    hdrs = glob(["**/*.h"], allow_empty = True ),
    include_prefix = "src/resource",
    deps = [
        "//api",
        "//sdk:headers",
        "//sdk/src/common:env_variables",
    ],
)
