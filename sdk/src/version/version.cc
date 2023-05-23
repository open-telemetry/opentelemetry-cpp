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
const int count_new_commits = 21;
const char *branch          = "make_release_1.9.1";
const char *commit_hash     = "61e8741a70cde8f273b3de49a772f05a12566e80";
const char *short_version   = "1.9.1";
const char *full_version =
    "1.9.1-NONE-NONE-21-make_release_1.9.1-61e8741a70cde8f273b3de49a772f05a12566e80";
const char *build_date = "Tue 23 May 2023 09:29:31 AM UTC";
}  // namespace version
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
