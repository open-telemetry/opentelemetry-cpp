# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

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
    deps = [
        "//ext:headers",
        "//ext/src/zpages",
        "//sdk/src/trace",
    ],
)
