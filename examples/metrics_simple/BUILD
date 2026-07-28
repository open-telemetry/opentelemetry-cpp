# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("@rules_cc//cc:cc_test.bzl", "cc_test")

cc_test(
    name = "metrics_ostream_example",
    size = "medium",
    srcs = [
        "metrics_ostream.cc",
    ],
    linkopts = ["-pthread"],
    tags = [
        "ostream",
        "tested_example",
    ],
    deps = [
        "//api",
        "//examples/common/metrics_foo_library:common_metrics_foo_library",
        "//exporters/ostream:ostream_metric_exporter",
        "//sdk/src/metrics",
    ],
)
