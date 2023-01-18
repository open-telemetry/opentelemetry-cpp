load("//:dll_deps.bzl", "dll_deps")

cc_binary(
    name = "example_multithreaded",
    srcs = [
        "main.cc",
    ],
    tags = ["ostream"],
    deps = dll_deps([
        "//api",
        "//exporters/ostream:ostream_span_exporter",
        "//sdk/src/trace",
    ]),
)
