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
    name = "recordable",
    srcs = [
        "recordable.cc",
    ],
    hdrs = [
        "recordable.h",
    ],
    include_prefix = "exporters/otlp",
    deps = [
        "//sdk/src/trace",
        "//sdk/src/common:random",
        "@com_github_opentelemetry_proto//:trace_proto_cc",
    ],
)

cc_library(
    name = "otlp_exporter",
    srcs = [
        'otlp_exporter.h',
        'otlp_exporter.cc',
    ],
    deps = [
        ":recordable",
        "//api",
        "//sdk/src/trace",
        "@com_github_opentelemetry_proto//:trace_proto_cc",
        "@com_github_opentelemetry_proto//:trace_service_proto_cc",

        # For gRPC
        "@com_github_opentelemetry_proto//:trace_service_grpc_cc",
        "@com_github_grpc_grpc//:grpc++",
    ],
)

cc_test(
    name = "recordable_test",
    srcs = ["recordable_test.cc"],
    deps = [
        ":recordable",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
 name = "otlp_exporter_test",
    srcs = ["otlp_exporter_test.cc"],
    deps = [
        ":otlp_exporter",
        "@com_google_googletest//:gtest_main",
    ],
)
