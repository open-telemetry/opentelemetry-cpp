load("//bazel:otel_cc_benchmark.bzl", "otel_cc_benchmark")

cc_test(
    name = "meter_provider_sdk_test",
    srcs = [
        "meter_provider_sdk_test.cc",
    ],
    tags = [
        "metrics",
        "test",
    ],
    deps = [
        "//sdk/src/metrics",
        "//sdk/src/resource",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "view_registry_test",
    srcs = [
        "view_registry_test.cc",
    ],
    tags = [
        "metrics",
        "test",
    ],
    deps = [
        "//sdk/src/metrics",
        "//sdk/src/resource",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "aggregation_test",
    srcs = [
        "aggregation_test.cc",
    ],
    tags = [
        "metrics",
        "test",
    ],
    deps = [
        "//sdk/src/metrics",
        "//sdk/src/resource",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "sync_metric_storage_test",
    srcs = [
        "sync_metric_storage_test.cc",
    ],
    tags = [
        "metrics",
        "test",
    ],
    deps = [
        "//sdk/src/metrics",
        "//sdk/src/resource",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "attributes_processor_test",
    srcs = [
        "attributes_processor_test.cc",
    ],
    tags = [
        "metrics",
        "test",
    ],
    deps = [
        "//sdk/src/metrics",
        "@com_google_googletest//:gtest_main",
    ],
)

otel_cc_benchmark(
    name = "attributes_processor_benchmark",
    srcs = [
        "attributes_processor_benchmark.cc",
    ],
    tags = [
        "metrics",
        "test",
    ],
    deps = [
        "//sdk/src/metrics",
    ],
)
