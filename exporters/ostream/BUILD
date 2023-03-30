# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "ostream_log_record_exporter",
    srcs = [
        "src/log_record_exporter.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/ostream/common_utils.h",
        "include/opentelemetry/exporters/ostream/log_record_exporter.h",
    ],
    strip_include_prefix = "include",
    tags = ["ostream"],
    deps = [
        "//sdk/src/logs",
    ],
)

cc_library(
    name = "ostream_metric_exporter",
    srcs = [
        "src/metric_exporter.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/ostream/common_utils.h",
        "include/opentelemetry/exporters/ostream/metric_exporter.h",
    ],
    strip_include_prefix = "include",
    tags = [
        "metrics",
        "ostream",
    ],
    deps = [
        "//sdk/src/metrics",
    ],
)

cc_library(
    name = "ostream_span_exporter",
    srcs = [
        "src/span_exporter.cc",
        "src/span_exporter_factory.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/ostream/common_utils.h",
        "include/opentelemetry/exporters/ostream/span_exporter.h",
        "include/opentelemetry/exporters/ostream/span_exporter_factory.h",
    ],
    strip_include_prefix = "include",
    tags = ["ostream"],
    deps = [
        "//sdk/src/trace",
    ],
)

cc_library(
    name = "headers",
    hdrs = glob(["include/**/*.h"]),
    strip_include_prefix = "include",
    deps = [
        "//sdk:headers",
    ],
)

# This is used for otel_sdk.zip packaging
filegroup(
    name="header_files",
    srcs=glob(["include/**/*.h"]),
)
