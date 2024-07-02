# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("//:dll_deps.bzl", "avoid_dll_lock", "dll_deps")
load("//bazel:otel_cc.bzl", "otel_cc_library", "otel_cc_binary", "otel_cc_test", "otel_cc_import", "otel_cc_shared_library")

avoid_dll_lock()

package(default_visibility = ["//visibility:public"])

otel_cc_library(
    name = "common_logs_foo_library",
    srcs = [
        "foo_library.cc",
    ],
    hdrs = [
        "foo_library.h",
    ],
    # TODO(malkia): Find out why include_prefix / strip_include_prefix do not work here!
    includes = [".."],
    deps = dll_deps([
        "//api",
        "//sdk:headers",
    ]),
)
