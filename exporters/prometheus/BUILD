# Copyright 2020, OpenTelemetry Authors
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "prometheus_exporter",
    srcs = [
        "src/prometheus_exporter.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/prometheus/prometheus_collector.h",
        "include/opentelemetry/exporters/prometheus/prometheus_exporter.h",
        "include/opentelemetry/exporters/prometheus/prometheus_exporter_utils.h",
    ],
    strip_include_prefix = "include",
    deps = [
        "//api",
        "//sdk:headers",
        "@com_github_jupp0r_prometheus_cpp//core",
        "@com_github_jupp0r_prometheus_cpp//pull",
    ],
)

cc_library(
    name = "prometheus_collector",
    srcs = [
        "src/prometheus_collector.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/prometheus/prometheus_collector.h",
        "include/opentelemetry/exporters/prometheus/prometheus_exporter_utils.h",
    ],
    strip_include_prefix = "include",
    deps = [
        ":prometheus_utils",
        "//api",
        "//sdk:headers",
        "@com_github_jupp0r_prometheus_cpp//core",
    ],
)

cc_library(
    name = "prometheus_utils",
    srcs = [
        "src/prometheus_exporter_utils.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/prometheus/prometheus_exporter_utils.h",
    ],
    strip_include_prefix = "include",
    deps = [
        "//api",
        "//sdk:headers",
        "@com_github_jupp0r_prometheus_cpp//core",
    ],
)

cc_test(
    name = "prometheus_exporter_test",
    srcs = [
        "test/prometheus_exporter_test.cc",
    ],
    deps = [
        ":prometheus_collector",
        ":prometheus_exporter",
        "@com_github_jupp0r_prometheus_cpp//core",
        "@com_github_jupp0r_prometheus_cpp//pull",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "prometheus_collector_test",
    srcs = [
        "test/prometheus_collector_test.cc",
    ],
    deps = [
        ":prometheus_collector",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "prometheus_exporter_utils_test",
    srcs = [
        "test/prometheus_exporter_utils_test.cc",
    ],
    deps = [
        ":prometheus_utils",
        "@com_google_googletest//:gtest_main",
    ],
)
