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

load("//bazel:otel_cc_benchmark.bzl", "otel_cc_benchmark")

cc_library(
    name = "recordable",
    srcs = [
        "src/recordable.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/otlp/protobuf_include_prefix.h",
        "include/opentelemetry/exporters/otlp/protobuf_include_suffix.h",
        "include/opentelemetry/exporters/otlp/recordable.h",
    ],
    strip_include_prefix = "include",
    deps = [
        "//sdk/src/trace",
        "@com_github_opentelemetry_proto//:trace_proto_cc",
    ],
)

cc_library(
    name = "otlp_exporter",
    srcs = [
        "src/otlp_exporter.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/otlp/protobuf_include_prefix.h",
        "include/opentelemetry/exporters/otlp/protobuf_include_suffix.h",
        "include/opentelemetry/exporters/otlp/otlp_exporter.h",
    ],
    strip_include_prefix = "include",
    deps = [
        ":recordable",
        "//sdk/src/trace",

        # For gRPC
        "@com_github_opentelemetry_proto//:trace_service_grpc_cc",
        "@com_github_grpc_grpc//:grpc++",
    ],
)

cc_test(
    name = "recordable_test",
    srcs = ["test/recordable_test.cc"],
    deps = [
        ":recordable",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "otlp_exporter_test",
    srcs = ["test/otlp_exporter_test.cc"],
    deps = [
        ":otlp_exporter",
        "//api",
        "@com_google_googletest//:gtest_main",
    ],
)

otel_cc_benchmark(
    name = "otlp_exporter_benchmark",
    srcs = ["test/otlp_exporter_benchmark.cc"],
    deps = [
        ":otlp_exporter",
    ],
)
