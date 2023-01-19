load("//:dll_deps.bzl", "dll_deps")

cc_binary(
    name = "prometheus_example",
    srcs = [
        "main.cc",
    ],
    linkopts = ["-pthread"],
    tags = ["prometheus"],
    deps = dll_deps([
        "//api",
        "//exporters/prometheus:prometheus_exporter",
        "//sdk/src/metrics",
    ]) + [
        "//examples/common/metrics_foo_library:common_metrics_foo_library",
        "@com_github_jupp0r_prometheus_cpp//core",
        "@com_github_jupp0r_prometheus_cpp//pull",        
    ],
)
