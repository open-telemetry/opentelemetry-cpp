# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load( "//:dll_deps.bzl", "dll_deps" )

package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "zpages_example",
    srcs = [
        "zpages_example.cc",
    ],
    linkopts = select({
        "//bazel:windows": [],
        "//conditions:default": ["-pthread"],
    }),
    tags = ["examples"],
    deps = dll_deps([
        "//ext:headers",
        "//ext/src/zpages",
        "//sdk/src/trace",
        "@github_nlohmann_json//:json"
    ]),
)
