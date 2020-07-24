package(default_visibility = ["//visibility:public"])

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
        "//sdk/src/trace",
    ],
)
