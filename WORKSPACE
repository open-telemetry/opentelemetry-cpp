# Copyright 2019, OpenTelemetry Authors
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

workspace(name = "io_opentelemetry_cpp")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# Load gRPC dependency
# Note that this dependency needs to be loaded first due to
# https://github.com/bazelbuild/bazel/issues/6664
http_archive(
    name = "com_github_grpc_grpc",
    sha256 = "d6277f77e0bb922d3f6f56c0f93292bb4cfabfc3c92b31ee5ccea0e100303612",
    strip_prefix = "grpc-1.28.0",
    urls = [
        "https://github.com/grpc/grpc/archive/v1.28.0.tar.gz",
    ],
)

load("@com_github_grpc_grpc//bazel:grpc_deps.bzl", "grpc_deps")

grpc_deps()

# Load extra gRPC dependencies due to https://github.com/grpc/grpc/issues/20511
load("@com_github_grpc_grpc//bazel:grpc_extra_deps.bzl", "grpc_extra_deps")

grpc_extra_deps()

load("@upb//bazel:repository_defs.bzl", "bazel_version_repository")

bazel_version_repository(name = "upb_bazel_version")

# Uses older protobuf version because of
# https://github.com/protocolbuffers/protobuf/issues/7179
http_archive(
    name = "com_google_protobuf",
    sha256 = "b679cef31102ed8beddc39ecfd6368ee311cbee6f50742f13f21be7278781821",
    strip_prefix = "protobuf-3.11.2",
    urls = [
        "https://github.com/protocolbuffers/protobuf/releases/download/v3.11.2/protobuf-all-3.11.2.tar.gz",
    ],
)

load("@com_google_protobuf//:protobuf_deps.bzl", "protobuf_deps")

protobuf_deps()

new_local_repository(
    name = "com_github_opentelemetry_proto",
    build_file = "//bazel:opentelemetry_proto.BUILD",
    path = "third_party/opentelemetry-proto",
)

# GoogleTest framework.
# Only needed for tests, not to build the OpenTelemetry library.
http_archive(
    name = "com_google_googletest",
    sha256 = "9dc9157a9a1551ec7a7e43daea9a694a0bb5fb8bec81235d8a1e6ef64c716dcb",
    strip_prefix = "googletest-release-1.10.0",
    urls = ["https://github.com/google/googletest/archive/release-1.10.0.tar.gz"],
)

# Google Benchmark library.
# Only needed for benchmarks, not to build the OpenTelemetry library.
http_archive(
    name = "com_github_google_benchmark",
    sha256 = "3c6a165b6ecc948967a1ead710d4a181d7b0fbcaa183ef7ea84604994966221a",
    strip_prefix = "benchmark-1.5.0",
    urls = ["https://github.com/google/benchmark/archive/v1.5.0.tar.gz"],
)

http_archive(
    name = "github_nlohmann_json",
    build_file = "//third_party/json:nlohmann_json.BUILD",
    sha256 = "69cc88207ce91347ea530b227ff0776db82dcb8de6704e1a3d74f4841bc651cf",
    urls = [
        "https://github.com/nlohmann/json/releases/download/v3.6.1/include.zip",
    ],
)

# C++ Prometheus Client library.
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive", "http_file")

http_archive(
    name = "com_github_jupp0r_prometheus_cpp",
    sha256 = "85ad6fea0f0dcb413104366b7d6109acdb015aff8767945511c5cad8202a28a6",
    strip_prefix = "prometheus-cpp-0.9.0",
    urls = ["https://github.com/jupp0r/prometheus-cpp/archive/v0.9.0.tar.gz"],
)

load("@com_github_jupp0r_prometheus_cpp//bazel:repositories.bzl", "prometheus_cpp_repositories")

prometheus_cpp_repositories()


# libcurl - An optional dependency we pull in for tests.
http_archive(
  name = "curl",
  urls = ["https://curl.haxx.se/download/curl-7.73.0.tar.gz"],
  strip_prefix = "curl-7.73.0",
  sha256 = "ba98332752257b47b9dea6d8c0ad25ec1745c20424f1dd3ff2c99ab59e97cf91",
  build_file = "@//third_party:curl.BUILD",
)
