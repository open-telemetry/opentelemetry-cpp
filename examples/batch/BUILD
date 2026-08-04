# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("@rules_cc//cc:cc_test.bzl", "cc_test")

cc_test(
    name = "example_simple",
    size = "medium",
    srcs = [
        "main.cc",
    ],
    linkopts = ["-lpthread"],
    tags = [
        "ostream",
        "tested_example",
    ],
    deps = [
        "//api",
        "//exporters/ostream:ostream_span_exporter",
        "//sdk/src/trace",
    ],
)
