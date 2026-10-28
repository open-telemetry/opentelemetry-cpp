# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("@rules_cc//cc:cc_library.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "instrumentationscope",
    srcs = [
        "instrumentation_scope.cc",
    ],
    include_prefix = "src/instrumentationscope",
    deps = [
        "//api",
        "//sdk:headers",
    ],
)
