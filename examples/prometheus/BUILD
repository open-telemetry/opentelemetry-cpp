# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

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
        "//examples/common/metrics_foo_library:common_metrics_foo_library",
        "//exporters/prometheus:prometheus_exporter",
        "//sdk/src/metrics",
        # These are not exported in the "dll" version, hence they need to be linked in statically
        # In the non-dll version, these are just duplicate deps, and would be ignored
        "@com_github_jupp0r_prometheus_cpp//core",
        "@com_github_jupp0r_prometheus_cpp//pull",
    ]),
)
