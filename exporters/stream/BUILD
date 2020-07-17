package(default_visibility = ["//visibility:public"])

cc_library(
    name = "stream_metrics_exporter",
    srcs = [
        "src/metrics_exporter.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/stream/metrics_exporter.h",
    ],
    strip_include_prefix = "include",
    deps = [
        "//sdk/src/metrics",
    ],
 )

cc_test(
    name = "stream_metrics_test",
    srcs = ["test/stream_metrics_test.cc"],
    deps = [
        ":stream_metrics_exporter",
        "@com_google_googletest//:gtest_main",
    ],
 )