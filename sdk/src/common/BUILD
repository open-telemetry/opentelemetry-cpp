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
    name = "random",
    srcs = [
        "core.cc",
        "random.cc",
    ],
    hdrs = [
        "fast_random_number_generator.h",
        "random.h",
    ],
    include_prefix = "src/common",
    deps = [
        "//api",
        "//sdk:headers",
        "//sdk/src/common/platform:fork",
    ],
)

cc_library(
    name = "global_log_handler",
    srcs = [
        "global_log_handler.cc",
    ],
    deps = [
        "//api",
        "//sdk:headers",
    ],
)
