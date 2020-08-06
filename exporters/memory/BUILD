package(default_visibility = ["//visibility:public"])

cc_library(
    name = "in_memory_span_data",
    srcs = [
        "src/in_memory_span_data.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/memory/in_memory_span_data.h",
    ],
    strip_include_prefix = "include",
    deps = [
        "//api",
        "//sdk/src/trace",
    ],
)

cc_test(
    name = "in_memory_span_data_test",
    srcs = ["test/in_memory_span_data_test.cc"],
    deps = [
        ":in_memory_span_data",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "in_memory_span_exporter",
    srcs = [
        "src/in_memory_span_exporter.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/memory/in_memory_span_exporter.h",
    ],
    strip_include_prefix = "include",
    deps = [
        ":in_memory_span_data",
        "//sdk/src/trace",
    ],
)

cc_test(
    name = "in_memory_span_exporter_test",
    srcs = ["test/in_memory_span_exporter_test.cc"],
    deps = [
        ":in_memory_span_exporter",
        "@com_google_googletest//:gtest_main",
    ],
)
