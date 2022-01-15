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

load("@bazel_skylib//rules:common_settings.bzl", "bool_flag")

package(default_visibility = ["//visibility:public"])

bool_flag(
    name = "with_abseil",
    build_setting_default = False,
)

cc_library(
    name = "api",
    hdrs = glob(["include/**/*.h"]),
    defines = select({
        ":with_external_abseil": ["HAVE_ABSEIL"],
        "//conditions:default": [],
    }),
    strip_include_prefix = "include",
    tags = ["api"],
    deps = select({
        ":with_external_abseil": [
            "@com_google_absl//absl/base",
            "@com_google_absl//absl/types:variant",
        ],
        "//conditions:default": [],
    }),
)

config_setting(
    name = "with_external_abseil",
    flag_values = {":with_abseil": "true"},
)
