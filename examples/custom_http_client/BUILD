# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("@rules_cc//cc:cc_test.bzl", "cc_test")

cc_library(
    name = "custom_http_client_stub",
    srcs = [
        "custom_http_client.cc",
    ],
    hdrs = [
        "custom_http_client.h",
    ],
    deps = [
        "//api",
        "//ext:headers",
        "//sdk:headers",
    ],
)

cc_test(
    name = "example_custom_http_client",
    srcs = [
        "main.cc",
    ],
    tags = [
        "otlp",
        "otlp_http",
        "tested_example",
    ],
    deps = [
        ":custom_http_client_stub",
        "//api",
        "//exporters/otlp:otlp_http_exporter",
        "//sdk/src/metrics",
        "//sdk/src/trace",
    ],
)
