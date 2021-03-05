load("//bazel:otel_cc_benchmark.bzl", "otel_cc_benchmark")

cc_test(
    name = "http_text_format_test",
    srcs = [
        "http_text_format_test.cc",
    ],
    deps = [
        "//api",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "jaeger_propagation_test",
    srcs = [
        "jaeger_propagation_test.cc",
    ],
    deps = [
        "//api",
        "@com_google_googletest//:gtest_main",
    ],
)
