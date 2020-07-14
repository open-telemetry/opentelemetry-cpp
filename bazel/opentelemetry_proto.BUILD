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

load("@rules_proto//proto:defs.bzl", "proto_library")
load("@com_github_grpc_grpc//bazel:cc_grpc_library.bzl", "cc_grpc_library")

proto_library(
    name = "common_proto",
    srcs = [
      "opentelemetry/proto/common/v1/common.proto",
    ],
)

cc_proto_library(
    name = "common_proto_cc",
    deps = [":common_proto"],
)

proto_library(
    name = "resource_proto",
    srcs = [
      "opentelemetry/proto/resource/v1/resource.proto",
    ],
    deps = [
      ":common_proto",
    ],
)

cc_proto_library(
    name = "resource_proto_cc",
    deps = [":resource_proto"],
)

proto_library(
    name = "trace_proto",
    srcs = [
      "opentelemetry/proto/trace/v1/trace.proto",
    ],
    deps = [
      ":common_proto",
      ":resource_proto",
    ],
)

cc_proto_library(
    name = "trace_proto_cc",
    deps = [":trace_proto"],
)

proto_library(
    name = "trace_service_proto",
    srcs = [
      "opentelemetry/proto/collector/trace/v1/trace_service.proto",
    ],
    deps = [
      ":trace_proto",
    ],
)

cc_proto_library(
    name = "trace_service_proto_cc",
    deps = [":trace_service_proto"],
)

cc_grpc_library(
    name = "trace_service_grpc_cc",
    srcs = [":trace_service_proto"],
    grpc_only = True,
    deps = [":trace_service_proto_cc"],
    generate_mocks = True,
)
