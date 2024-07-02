# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("//:dll_deps.bzl", "dll_deps", "avoid_dll_lock")
load("//bazel:otel_cc.bzl", "otel_cc_library", "otel_cc_binary", "otel_cc_test", "otel_cc_import", "otel_cc_shared_library")

avoid_dll_lock()

otel_cc_binary(
    name = "metrics_ostream_example",
    srcs = [
        "metrics_ostream.cc",
    ],
    linkopts = ["-pthread"],
    tags = ["ostream"],
    deps = dll_deps([
        "//api",
        "//examples/common/metrics_foo_library:common_metrics_foo_library",
        "//exporters/ostream:ostream_metric_exporter",
        "//sdk/src/metrics",
    ]),
)
