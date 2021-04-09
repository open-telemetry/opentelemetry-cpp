package(default_visibility = ["//visibility:public"])

cc_library(
    name = "ostream_log_exporter",
    srcs = [
        "src/log_exporter.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/ostream/log_exporter.h",
    ],
    strip_include_prefix = "include",
    deps = [
        "//sdk/src/logs",
    ],
)

cc_test(
    name = "ostream_log_test",
    srcs = ["test/ostream_log_test.cc"],
    deps = [
        ":ostream_log_exporter",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "ostream_metrics_exporter",
    srcs = [
        "src/metrics_exporter.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/ostream/metrics_exporter.h",
    ],
    strip_include_prefix = "include",
    deps = [
        "//sdk/src/metrics",
    ],
)

cc_test(
    name = "ostream_metrics_test",
    srcs = ["test/ostream_metrics_test.cc"],
    deps = [
        ":ostream_metrics_exporter",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "ostream_span_exporter",
    srcs = [
        "src/span_exporter.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/ostream/span_exporter.h",
    ],
    strip_include_prefix = "include",
    deps = [
        "//sdk/src/trace",
    ],
)

cc_library(
    name = "ostream_capture",
    hdrs = [
        "test/ostream_capture.h",
    ],
    deps = [
        "//api",
    ],
)

cc_test(
    name = "ostream_span_test",
    srcs = ["test/ostream_span_test.cc"],
    deps = [
        ":ostream_capture",
        ":ostream_span_exporter",
        "@com_google_googletest//:gtest_main",
    ],
)
