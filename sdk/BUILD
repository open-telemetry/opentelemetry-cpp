# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("//bazel:otel_cc.bzl", "otel_cc_library", "otel_cc_binary", "otel_cc_test", "otel_cc_import", "otel_cc_shared_library")

package(default_visibility = ["//visibility:public"])

otel_cc_library(
    name = "headers",
    hdrs = glob(["include/**/*.h"]),
    strip_include_prefix = "include",
)

# This is used for otel_sdk.zip packaging
filegroup(
    name="header_files",
    srcs=glob(["include/**/*.h"]),
)
