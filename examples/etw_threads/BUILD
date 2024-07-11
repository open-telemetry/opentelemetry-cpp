# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("//:dll_deps.bzl", "dll_deps")
load("//bazel:otel_cc.bzl", "otel_cc_binary", "otel_cc_import", "otel_cc_library", "otel_cc_shared_library", "otel_cc_test")

otel_cc_binary(
    name = "etw_threadpool",
    srcs = [
        "main.cc",
    ],
    tags = [
        "etw",
        "examples",
    ],
    deps = dll_deps([
        "//exporters/etw:etw_exporter",
        "//:windows_only",
    ]),
)
