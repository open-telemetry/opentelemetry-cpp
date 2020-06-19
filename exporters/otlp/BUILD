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
        "@com_github_opentelemetry_proto//:trace_proto_cc",
    ],
)

cc_library(
    name = "foo_library",
    srcs = [
        "foo_library/foo_library.cc",
    ],
    hdrs = [
        "foo_library/foo_library.h",
    ],
    deps = [
        "//api",
    ],
)

cc_binary(
    name = "example_simple",
    srcs = [
        "main.cc",
        #"stdout_exporter.h",
        'otlp_exporter.h',
    ],
    deps = [
        ":foo_library",
        "//api",
        "//sdk/src/trace",
        "@com_github_opentelemetry_proto//:trace_proto_cc",
        "@com_github_opentelemetry_proto//:trace_service_proto_cc",
        #"@com_github_opentelemetry_proto//:trace_service_grpc_cc",
        #"@com_github_grpc_grpc//:grpc++",
    ],
)

