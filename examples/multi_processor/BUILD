load("//:dll_deps.bzl", "dll_deps")

cc_binary(
    name = "example_multi_processor",
    srcs = [
        "main.cc",
    ],
    tags = [
        "memory",
        "ostream",
    ],
    deps = dll_deps([
        "//api",
        "//exporters/memory:in_memory_span_exporter",
        "//exporters/ostream:ostream_span_exporter",
        "//sdk/src/trace",
    ]) + [
        "//examples/common/foo_library:common_foo_library",
    ],
)
