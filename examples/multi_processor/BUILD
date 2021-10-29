cc_binary(
    name = "example_multi_processor",
    srcs = [
        "main.cc",
    ],
    deps = [
        "//examples/common/foo_library:common_foo_library",
        "//api",
        "//exporters/memory:in_memory_span_exporter",
        "//exporters/ostream:ostream_span_exporter",
        "//sdk/src/trace",
    ],
)
