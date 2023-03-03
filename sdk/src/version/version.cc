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
const int minor_version     = 8;
const int patch_version     = 2;
const char *pre_release     = "NONE";
const char *build_metadata  = "NONE";
const int count_new_commits = 37;
const char *branch          = "pre_release_1.8.2";
const char *commit_hash     = "435ce60f233b6718aaa04bb7068dd641b536299b";
const char *short_version   = "1.8.2";
const char *full_version =
    "1.8.2-NONE-NONE-37-pre_release_1.8.2-435ce60f233b6718aaa04bb7068dd641b536299b";
const char *build_date = "Tue 31 Jan 2023 04:01:10 PM UTC";
}  // namespace version
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
