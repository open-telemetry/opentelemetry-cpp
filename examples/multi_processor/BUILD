cc_library(
    name = "foo_multi_library",
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
    name = "example_multi_processor",
    srcs = [
        "main.cc",
    ],
    deps = [
        ":foo_multi_library",
        "//api",
        "//exporters/ostream:ostream_span_exporter",
        "//sdk/src/trace",
    ],
)
