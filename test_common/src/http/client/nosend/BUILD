# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("//bazel:otel_cc.bzl", "otel_cc_library", "otel_cc_binary", "otel_cc_test", "otel_cc_import", "otel_cc_shared_library")

package(default_visibility = ["//visibility:public"])

otel_cc_library(
    name = "http_client_nosend",
    srcs = [
        "http_client_nosend.cc",
        "http_client_test_factory.cc",
    ],
    include_prefix = "src/http/client/nosend",
    tags = [
        "test",
    ],
    deps = [
        "//api",
        "//ext:headers",
        "//sdk:headers",
        "//test_common:headers",
        "@googletest//:gtest_main",
    ],
)
