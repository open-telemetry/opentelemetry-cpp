cc_binary(
    name = "metrics_simple_example",
    srcs = [
        "main.cc",
    ],
    linkopts = ["-pthread"],
    deps = [
        "//api",
        "//exporters/ostream:ostream_metrics_exporter",
        "//sdk/src/metrics",
    ],
)
