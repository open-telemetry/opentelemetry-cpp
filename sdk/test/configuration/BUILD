# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

cc_test(
    name = "yaml_test",
    srcs = [
        "yaml_test.cc",
    ],
    tags = [
        "test",
        "yaml",
    ],
    deps = [
        "//api",
        "//sdk:headers",
        "//sdk/src/configuration",
        "//sdk/src/logs",
        "//sdk/src/metrics",
        "//sdk/src/trace",
        "@com_google_googletest//:gtest_main",
    ],
)
