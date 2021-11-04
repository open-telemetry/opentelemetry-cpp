cc_binary(
    name = "metrics_simple_example",
    srcs = [
        "main.cc",
    ],
    linkopts = ["-pthread"],
    deps = [
        "//api",
        "//exporters/ostream:ostream_metrics_exporter_deprecated",
	"//sdk/src/_metrics:metrics_deprecated",
    ],
)
