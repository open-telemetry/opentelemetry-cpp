cc_binary(
    name = "metrics_simple_example",
    srcs = [
        "main.cc",
    ],
    linkopts = ["-pthread"],
    tags = ["ostream"],
    deps = [
        "//api",
        "//exporters/ostream:ostream_metrics_exporter_deprecated",
        "//sdk/src/_metrics:metrics_deprecated",
    ],
)

# TODO - Uncomment once MetricData interface is finalised
# cc_binary(
#     name = "metrics_ostream_example",
#     srcs = [
#         "metrics_ostream.cc",
#     ],
#     linkopts = ["-pthread"],
#     tags = ["ostream"],
#     deps = [
#         "//api",
#         "//exporters/ostream:ostream_metric_exporter",
#         "//sdk/src/metrics",
#     ],
# )
