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

cc_test(
    name = "recordable_test",
    srcs = ["recordable_test.cc"],
    deps = [
        ":recordable",
        "@com_google_googletest//:gtest_main",
    ],
)
