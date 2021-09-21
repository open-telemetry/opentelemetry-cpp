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
    name = "otlp_recordable",
    srcs = [
        "src/otlp_recordable.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/otlp/otlp_recordable.h",
        "include/opentelemetry/exporters/otlp/protobuf_include_prefix.h",
        "include/opentelemetry/exporters/otlp/protobuf_include_suffix.h",
    ],
    strip_include_prefix = "include",
    deps = [
        "//sdk/src/resource",
        "//sdk/src/trace",
        "@com_github_opentelemetry_proto//:trace_proto_cc",
    ],
)

cc_library(
    name = "otlp_grpc_exporter",
    srcs = [
        "src/otlp_grpc_exporter.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/otlp/otlp_environment.h",
        "include/opentelemetry/exporters/otlp/otlp_grpc_exporter.h",
        "include/opentelemetry/exporters/otlp/protobuf_include_prefix.h",
        "include/opentelemetry/exporters/otlp/protobuf_include_suffix.h",
    ],
    strip_include_prefix = "include",
    deps = [
        ":otlp_recordable",
        "//sdk/src/trace",

        # For gRPC
        "@com_github_opentelemetry_proto//:trace_service_grpc_cc",
        "@com_github_grpc_grpc//:grpc++",
    ],
)

cc_library(
    name = "otlp_http_exporter",
    srcs = [
        "src/otlp_http_exporter.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/otlp/otlp_environment.h",
        "include/opentelemetry/exporters/otlp/otlp_http_exporter.h",
        "include/opentelemetry/exporters/otlp/protobuf_include_prefix.h",
        "include/opentelemetry/exporters/otlp/protobuf_include_suffix.h",
    ],
    copts = [
        "-DCURL_STATICLIB",
    ],
    linkopts = select({
        "//bazel:windows": [
            "-DEFAULTLIB:advapi32.lib",
            "-DEFAULTLIB:crypt32.lib",
        ],
        "//conditions:default": [],
    }),
    strip_include_prefix = "include",
    deps = [
        ":otlp_recordable",
        "//ext/src/http/client/curl:http_client_curl",
        "//sdk/src/trace",
        "@com_github_opentelemetry_proto//:trace_service_proto_cc",
        "@github_nlohmann_json//:json",
    ],
)

cc_test(
    name = "otlp_recordable_test",
    srcs = ["test/otlp_recordable_test.cc"],
    deps = [
        ":otlp_recordable",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "otlp_grpc_exporter_test",
    srcs = ["test/otlp_grpc_exporter_test.cc"],
    deps = [
        ":otlp_grpc_exporter",
        "//api",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "otlp_http_exporter_test",
    srcs = ["test/otlp_http_exporter_test.cc"],
    deps = [
        ":otlp_http_exporter",
        "//api",
        "@com_google_googletest//:gtest_main",
    ],
)

otel_cc_benchmark(
    name = "otlp_grpc_exporter_benchmark",
    srcs = ["test/otlp_grpc_exporter_benchmark.cc"],
    deps = [
        ":otlp_grpc_exporter",
    ],
)
