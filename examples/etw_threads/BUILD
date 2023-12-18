# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("//:dll_deps.bzl", "dll_deps", "avoid_dll_lock")

avoid_dll_lock()

cc_binary(
    name = "etw_threadpool",
    srcs = [
        "main.cc",
    ],
    tags = [
        "examples",
        "etw",
    ],
    deps = dll_deps([
        "//exporters/etw:etw_exporter",
    ]),
)
