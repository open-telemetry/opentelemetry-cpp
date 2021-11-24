cc_binary(
    name = "example_simple",
    srcs = [
        "main.cc",
    ],
    linkopts = ["-lpthread"],
    tags = ["ostream"],
    deps = [
        "//api",
        "//exporters/ostream:ostream_span_exporter",
        "//sdk/src/trace",
    ],
)
