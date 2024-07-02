# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("//:dll_deps.bzl", "dll_deps", "avoid_dll_lock")
load("//bazel:otel_cc.bzl", "otel_cc_library", "otel_cc_binary", "otel_cc_test", "otel_cc_import", "otel_cc_shared_library")

avoid_dll_lock()

otel_cc_binary(
    name = "load_plugin",
    srcs = [
        "main.cc",
    ],
    linkopts = [
        "-ldl",
    ],
    deps = dll_deps([
        "//api",
        "//examples/plugin/plugin:example_plugin.so",
    ]),
    args = [
        "$(rootpath //examples/plugin/plugin:example_plugin.so)",
        # This argument here is a dummy config file, that's been read and ignored
        "$(rootpath //examples/plugin/plugin:example_plugin.so)",
    ]
)

