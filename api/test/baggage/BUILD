load("//bazel:otel_cc_benchmark.bzl", "otel_cc_benchmark")

cc_test(
    name = "baggage_test",
    srcs = [
        "baggage_test.cc",
    ],
    deps = [
        "//api",
        "@com_google_googletest//:gtest_main",
    ],
)

otel_cc_benchmark(
    name = "baggage_benchmark",
    srcs = ["baggage_benchmark.cc"],
    deps = ["//api"],
)
