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
const int patch_version     = 1;
const char *pre_release     = "NONE";
const char *build_metadata  = "NONE";
const int count_new_commits = 22;
const char *branch          = "make_release_1.9.1";
const char *commit_hash     = "5592180d539b59c4e8293bc927f5a6431fcbacdf";
const char *short_version   = "1.9.1";
const char *full_version =
    "1.9.1-NONE-NONE-22-make_release_1.9.1-5592180d539b59c4e8293bc927f5a6431fcbacdf";
const char *build_date = "Fri 26 May 2023 07:14:07 AM UTC";
}  // namespace version
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
