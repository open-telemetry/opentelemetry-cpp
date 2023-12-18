# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("//:dll_deps.bzl", "dll_deps", "avoid_dll_lock")

avoid_dll_lock()

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "common_logs_foo_library",
    srcs = [
        "foo_library.cc",
    ],
    hdrs = [
        "foo_library.h",
    ],
    defines = ["BAZEL_BUILD"],
    deps = dll_deps([
        "//api",
        "//sdk:headers",
    ]),
)
