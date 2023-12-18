# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("//bazel:otel_cc_benchmark.bzl", "otel_cc_benchmark")
load("//:dll_deps.bzl", "dll_deps", "avoid_dll_lock")

avoid_dll_lock()

package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "func_otlp_http",
    srcs = [
        "func_http_main.cc"
    ],
    tags = ["otlp"],
    deps = dll_deps([
        "//exporters/otlp:otlp_http_exporter",
    ]),
)
