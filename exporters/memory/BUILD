package(default_visibility = ["//visibility:public"])

cc_library(
    name = "in_memory_span_data",
    hdrs = [
        "include/opentelemetry/exporters/memory/in_memory_span_data.h",
    ],
    strip_include_prefix = "include",
    tags = ["memory"],
    deps = [
        "//api",
        "//sdk/src/resource",
        "//sdk/src/trace",
    ],
)

cc_test(
    name = "in_memory_span_data_test",
    srcs = ["test/in_memory_span_data_test.cc"],
    tags = [
        "memory",
        "test",
    ],
    deps = [
        ":in_memory_span_data",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "in_memory_span_exporter",
    hdrs = [
        "include/opentelemetry/exporters/memory/in_memory_span_exporter.h",
    ],
    strip_include_prefix = "include",
    tags = [
        "memory",
        "test",
    ],
    deps = [
        ":in_memory_span_data",
        "//sdk/src/trace",
    ],
)

cc_test(
    name = "in_memory_span_exporter_test",
    srcs = ["test/in_memory_span_exporter_test.cc"],
    tags = [
        "memory",
        "test",
    ],
    deps = [
        ":in_memory_span_exporter",
        "@com_google_googletest//:gtest_main",
    ],
)
