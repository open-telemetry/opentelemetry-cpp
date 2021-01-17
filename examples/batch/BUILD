cc_binary(
    name = "example_simple",
    srcs = [
        "main.cc",
    ],
    linkopts = ["-lpthread"],
    deps = [
        "//api",
        "//exporters/ostream:ostream_span_exporter",
        "//sdk/src/resource",
        "//sdk/src/trace",
    ],
)
