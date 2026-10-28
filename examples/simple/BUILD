# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("@rules_cc//cc:cc_test.bzl", "cc_test")

cc_test(
    name = "example_simple",
    srcs = [
        "main.cc",
    ],
    tags = [
        "examples",
        "ostream",
        "tested_example",
    ],
    deps = [
        "//api",
        "//examples/common/foo_library:common_foo_library",
        "//exporters/ostream:ostream_span_exporter",
    ],
)
