package(default_visibility = ["//visibility:public"])

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

cc_test(
    name = "ostream_span_test",
    srcs = ["test/ostream_span_test.cc"],
    deps = [
        ":ostream_span_exporter",
        "@com_google_googletest//:gtest_main",
    ],
)
