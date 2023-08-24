# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "zpages",
    srcs = glob(["**/*.cc"]),
    hdrs = glob(["**/*.h"]),
    include_prefix = "ext/zpages",
    deps = [
        "//api",
        "//ext:headers",
        "//sdk:headers",
        "@github_nlohmann_json//:json",
    ],
)
