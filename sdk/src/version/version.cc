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
const int major_version = 1;
const int minor_version = 8;
const int patch_version = 3;
const char* pre_release = "NONE";
const char* build_metadata = "NONE";
const int count_new_commits = 33;
const char* branch = "pre_release_1.8.3";
const char* commit_hash = "267c28ff22c5c380eb1897f35995961223f43a72";
const char* short_version = "1.8.3";
const char* full_version =
    "1.8.2-NONE-NONE-33-pre_release_1.8.3-267c28ff22c5c380eb1897f35995961223f43a72";
const char* build_date = "Mon Mar 6 20:46:20 UTC 2023";
}  // namespace version
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
