// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

// Please DONOT touch this file.
// Any changes done here would be overwritten by pre-commit git hook

#include "opentelemetry/sdk/version/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace version
{
const int major_version     = 1;
const int minor_version     = 9;
const int patch_version     = 0;
const char *pre_release     = "NONE";
const char *build_metadata  = "NONE";
const int count_new_commits = 23;
const char *branch          = "pre_release_1.9.0";
const char *commit_hash     = "b2913ce493075f11d3891c1dc97d908b2aac4d14";
const char *short_version   = "1.9.0";
const char *full_version =
    "1.9.0-NONE-NONE-23-pre_release_1.9.0-b2913ce493075f11d3891c1dc97d908b2aac4d14";
const char *build_date = "Wed 12 Apr 2023 10:33:02 AM UTC";
}  // namespace version
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
