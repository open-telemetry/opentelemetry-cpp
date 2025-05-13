# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("//bazel:otel_cc.bzl", "otel_cc_binary")
load("//:dll_deps.bzl", "dll_deps")

package(default_visibility = ["//visibility:public"])

otel_cc_binary(
    name = "func_otlp_http",
    srcs = [
        "func_http_main.cc",
    ],
    args = [
        "--list",
    ],
    tags = [
        "otlp",
        "otlp_http",
    ],
    deps = dll_deps([
        "//sdk/src/metrics",
        "//exporters/otlp:otlp_http_exporter",
    ]),
)

otel_cc_binary(
    name = "func_otlp_grpc",
    srcs = [
        "func_grpc_main.cc",
    ],
    args = [
        "--list",
    ],
    tags = [
        "otlp",
        "otlp_grpc",
    ],
    deps = dll_deps([
        "//sdk/src/metrics",
        "//exporters/otlp:otlp_grpc_exporter",
    ]),
)
