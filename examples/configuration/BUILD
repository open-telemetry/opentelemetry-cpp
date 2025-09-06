# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

cc_test(
    name = "example_yaml",
    srcs = glob(["*.cc"]) + glob(["*.h"]),
    tags = [
        "test",
        "yaml",
    ],
    deps = [
        "//api",
        "//examples/common/logs_foo_library:common_logs_foo_library",
        "//examples/common/metrics_foo_library:common_metrics_foo_library",
        "//exporters/ostream:ostream_log_record_exporter_builder",
        "//exporters/ostream:ostream_metric_exporter_builder",
        "//exporters/ostream:ostream_span_exporter_builder",
        "//sdk:headers",
        "//sdk/src/configuration",
        "//sdk/src/logs",
        "//sdk/src/metrics",
        "//sdk/src/trace",
        "@com_google_googletest//:gtest_main",
    ],
)
