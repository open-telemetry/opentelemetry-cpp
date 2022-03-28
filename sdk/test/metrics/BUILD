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
    name = "metric_reader_test",
    srcs = [
        "metric_reader_test.cc",
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
    name = "sync_instruments_test",
    srcs = [
        "sync_instruments_test.cc",
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
    name = "async_metric_storage_test",
    srcs = [
        "async_metric_storage_test.cc",
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
    name = "observer_result_test",
    srcs = [
        "observer_result_test.cc",
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
    name = "multi_metric_storage_test",
    srcs = [
        "multi_metric_storage_test.cc",
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

cc_test(
    name = "attributes_hashmap_test",
    srcs = [
        "attributes_hashmap_test.cc",
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

otel_cc_benchmark(
    name = "attributes_hashmap_benchmark",
    srcs = [
        "attributes_hashmap_benchmark.cc",
    ],
    tags = [
        "metrics",
        "test",
    ],
    deps = [
        "//sdk/src/metrics",
    ],
)
