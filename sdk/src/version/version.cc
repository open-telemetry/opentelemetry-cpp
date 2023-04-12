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
const int count_new_commits = 26;
const char *branch          = "pre_release_1.9.0";
const char *commit_hash     = "a9876353337d5de74323b8935479618c9b422d09";
const char *short_version   = "1.9.0";
const char *full_version =
    "1.9.0-NONE-NONE-26-pre_release_1.9.0-a9876353337d5de74323b8935479618c9b422d09";
const char *build_date = "Wed 12 Apr 2023 03:32:28 PM UTC";
}  // namespace version
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
