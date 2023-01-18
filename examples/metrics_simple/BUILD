load("//:dll_deps.bzl", "dll_deps")

cc_binary(
    name = "metrics_ostream_example",
    srcs = [
        "metrics_ostream.cc",
    ],
    linkopts = ["-pthread"],
    tags = ["ostream"],
    deps = dll_deps([
        "//api",
        "//exporters/ostream:ostream_metric_exporter",
        "//sdk/src/metrics",
    ]) + [
        "//examples/common/metrics_foo_library:common_metrics_foo_library",
    ],
)
