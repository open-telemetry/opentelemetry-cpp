# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load( "//:dll_deps.bzl", "dll_deps" )

package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "example_zipkin",
    srcs = [
        "main.cc",
    ],
    linkopts = select({
        "//bazel:windows": [],
        "//conditions:default": ["-pthread"],
    }),
    tags = ["examples", "zipkin"],
    deps = dll_deps([
        "//examples/common/foo_library:common_foo_library",
        "//exporters/zipkin:zipkin_exporter"
    ]),
)
