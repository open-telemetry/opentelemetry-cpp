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

http_archive(
    name = "bazel_skylib",
    sha256 = "64ad2728ccdd2044216e4cec7815918b7bb3bb28c95b7e9d951f9d4eccb07625",
    strip_prefix = "bazel-skylib-1.0.2",
    urls = [
        "https://github.com/bazelbuild/bazel-skylib/archive/1.0.2.zip",
    ],
)

http_archive(
    name = "rules_foreign_cc",
    strip_prefix = "rules_foreign_cc-456425521973736ef346d93d3d6ba07d807047df",
    url = "https://github.com/bazelbuild/rules_foreign_cc/archive/456425521973736ef346d93d3d6ba07d807047df.zip",
)

load("@rules_foreign_cc//:workspace_definitions.bzl", "rules_foreign_cc_dependencies")

rules_foreign_cc_dependencies([
])

http_archive(
    name = "com_github_libevent_libevent",
    build_file = "//bazel:libevent.BUILD",
    sha256 = "70158101eab7ed44fd9cc34e7f247b3cae91a8e4490745d9d6eb7edc184e4d96",
    strip_prefix = "libevent-release-2.1.8-stable",
    urls = [
        "https://github.com/libevent/libevent/archive/release-2.1.8-stable.zip",
    ],
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
