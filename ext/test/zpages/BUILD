load("//bazel:otel_cc_benchmark.bzl", "otel_cc_benchmark")

cc_test(
    name = "threadsafe_span_data_test",
    srcs = [
        "threadsafe_span_data_test.cc",
    ],
    deps = [
        "//ext/src/zpages",
        "//sdk/src/trace",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "tracez_data_aggregator_test",
    srcs = [
        "tracez_data_aggregator_test.cc",
    ],
    deps = [
        "//ext/src/zpages",
        "//sdk/src/trace",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "tracez_processor_test",
    srcs = [
        "tracez_processor_test.cc",
    ],
    deps = [
        "//ext/src/zpages",
        "//sdk/src/trace",
        "@com_google_googletest//:gtest_main",
    ],
)

otel_cc_benchmark(
    name = "tracez_processor_benchmark",
    srcs = [
        "tracez_processor_benchmark.cc",
    ],
    deps = [
        "//ext/src/zpages",
        "//sdk/src/trace",
        "//api",
    ],
)
