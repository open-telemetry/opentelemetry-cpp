# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("//:dll_deps.bzl", "dll_deps")

package(default_visibility = ["//visibility:public"])

cc_test(
    name = "ostream_log_test",
    srcs = ["ostream_log_test.cc"],
    tags = [
        "ostream",
        "test",
    ],
    deps = dll_deps([
        "//exporters/ostream:ostream_log_record_exporter",
        "@com_google_googletest//:gtest_main",
    ]),
)

cc_test(
    name = "ostream_metric_test",
    srcs = ["ostream_metric_test.cc"],
    tags = [
        "ostream",
        "test",
    ],
    deps = dll_deps([
        "//exporters/ostream:ostream_metric_exporter",
        "@com_google_googletest//:gtest_main",
    ]),
)

cc_library(
    name = "ostream_capture",
    hdrs = [
        "ostream_capture.h",
    ],
    tags = ["ostream"],
    deps = dll_deps([
        "//api",
    ]),
)

cc_test(
    name = "ostream_span_test",
    srcs = ["ostream_span_test.cc"],
    tags = [
        "ostream",
        "test",
    ],
    deps = dll_deps([
        "//exporters/ostream:ostream_span_exporter",
        ":ostream_capture",
        "@com_google_googletest//:gtest_main",
    ]),
)
