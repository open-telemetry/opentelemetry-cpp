cc_library(
    name = "foo_library",
    srcs = [
        "foo_library/foo_library.cc",
    ],
    hdrs = [
        "foo_library/foo_library.h",
    ],
    deps = [
        "//api",
    ],
)

cc_binary(
    name = "example_simple",
    srcs = [
        "main.cc",
    ],
    deps = [
        ":foo_library",
        "//exporters/ostream:ostream_span_exporter",
        "//api",
        "//sdk/src/trace",
    ],
)
