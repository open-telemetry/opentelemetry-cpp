load("//bazel:otel_cc_benchmark.bzl", "otel_cc_benchmark")

cc_test(
    name = "meter_provider_test",
    srcs = [
        "meter_provider_test.cc",
    ],
    tags = [
        "api",
        "metrics",
        "test",
    ],
    deps = [
        "//api",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "noop_metrics_test",
    srcs = [
        "noop_metrics_test.cc",
    ],
    tags = [
        "api",
        "metrics",
        "test",
    ],
    deps = [
        "//api",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "noop_instrument_test",
    srcs = [
        "noop_instrument_test.cc",
    ],
    linkstatic = 1,
    tags = [
        "api",
        "metrics",
        "test",
    ],
    deps = [
        "//api",
        "@com_google_googletest//:gtest_main",
    ],
)
