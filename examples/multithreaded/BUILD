# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("//:dll_deps.bzl", "dll_deps", "avoid_dll_lock")
load("//bazel:otel_cc.bzl", "otel_cc_library", "otel_cc_binary", "otel_cc_test", "otel_cc_import", "otel_cc_shared_library")

avoid_dll_lock()

otel_cc_binary(
    name = "example_multithreaded",
    srcs = [
        "main.cc",
    ],
    tags = ["ostream"],
    deps = dll_deps([
        "//api",
        "//exporters/ostream:ostream_span_exporter",
        "//sdk/src/trace",
    ]),
)
