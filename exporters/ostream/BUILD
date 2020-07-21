package(default_visibility = ["//visibility:public"])

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