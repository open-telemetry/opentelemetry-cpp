# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("//bazel:otel_cc.bzl", "otel_cc_library", "otel_cc_binary", "otel_cc_test", "otel_cc_import", "otel_cc_shared_library")

package(default_visibility = ["//visibility:public"])

otel_cc_library(
    name = "fork",
    srcs = select({
        "//bazel:windows": ["fork_windows.cc"],
        "//conditions:default": ["fork_unix.cc"],
    }),
    hdrs = [
        "fork.h",
    ],
    include_prefix = "src/common/platform",
    linkopts = select({
        "//bazel:windows": [],
        "//conditions:default": ["-pthread"],
    }),
    deps = [
        "//api",
    ],
)
