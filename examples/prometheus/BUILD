cc_binary(
    name = "prometheus_example",
    srcs = [
        "main.cc",
    ],
    linkopts = ["-pthread"],
    tags = ["ostream"],
    deps = [
        "//api",
        "//examples/common/metrics_foo_library:common_metrics_foo_library",
        "//exporters/prometheus:prometheus_exporter",
        "//sdk/src/metrics",
    ],
)
