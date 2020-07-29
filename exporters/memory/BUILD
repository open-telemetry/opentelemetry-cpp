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
        "//sdk/src/trace",
        "//sdk/src/common:atomic_unique_ptr",
        "//sdk/src/common:circular_buffer",
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
