# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("@rules_cc//cc:cc_library.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "headers",
    hdrs = glob(["include/**/*.h"]),
    linkopts = select({
        # socket_tools.h calls Winsock; declare it on the interface so consumers link it too.
        "//bazel:windows": ["-DEFAULTLIB:Ws2_32.lib"],
        "//conditions:default": [],
    }),
    strip_include_prefix = "include",
)
