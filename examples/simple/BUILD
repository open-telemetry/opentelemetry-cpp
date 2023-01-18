load( "//:dll_deps.bzl", "dll_deps" )

cc_binary(
    name = "example_simple",
    srcs = [
        "main.cc",
    ],
    tags = [
        "examples",
        "ostream",
    ],
    deps = dll_deps([
        "//api",
        "//exporters/ostream:ostream_span_exporter",
    ]) + [
        "//examples/common/foo_library:common_foo_library",
    ],
)
