# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("//:dll_deps.bzl", "dll_deps")
load("//bazel:otel_cc.bzl", "otel_cc_binary")

otel_cc_binary(
    name = "example_plugin.so",
    srcs = [
        "factory_impl.cc",
        "tracer.cc",
        "tracer.h",
    ],
    linkshared = 1,
    visibility = [
        "//examples/plugin/load:__pkg__",
    ],
    deps = dll_deps([
        "//api",
    ]),
)
