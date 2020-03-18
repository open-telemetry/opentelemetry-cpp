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
    name = "atomic_unique_ptr",
    hdrs = [
        "atomic_unique_ptr.h",
    ],
    include_prefix = "src/common",
    deps = [
        "//api",
    ],
)

cc_library(
    name = "circular_buffer_range",
    hdrs = [
        "circular_buffer_range.h",
    ],
    include_prefix = "src/common",
    deps = [
        "//api",
    ],
)

cc_library(
    name = "circular_buffer",
    hdrs = [
        "circular_buffer.h",
    ],
    include_prefix = "src/common",
    deps = [
        ":atomic_unique_ptr",
        ":circular_buffer_range",
        "//api",
    ],
)
