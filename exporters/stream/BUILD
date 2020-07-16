package(default_visibility = ["//visibility:public"])

cc_library(
    name = "stream_span_exporter",
    srcs = [
        "src/span_exporter.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/stream/span_exporter.h",
    ],
    strip_include_prefix = "include",
    deps = [
        "//sdk/src/trace",
    ],
)

cc_test(
    name = "stream_span_test",
    srcs = ["test/stream_span_test.cc"],
    deps = [
        ":stream_span_exporter",
        "@com_google_googletest//:gtest_main",
    ],
)
