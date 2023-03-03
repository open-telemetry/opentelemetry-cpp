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
const int OPENTELEMETRY_SDK_MAJOR_VERSION     = 1;
const int OPENTELEMETRY_SDK_MINOR_VERSION     = 8;
const int OPENTELEMETRY_SDK_PATCH_VERSION     = 2;
const char *OPENTELEMETRY_SDK_PRE_RELEASE     = "NONE";
const char *OPENTELEMETRY_SDK_BUILD_METADATA  = "NONE";
const int OPENTELEMETRY_SDK_COUNT_NEW_COMMITS = 37;
const char *OPENTELEMETRY_SDK_BRANCH          = "pre_release_1.8.2";
const char *OPENTELEMETRY_SDK_COMMIT_HASH     = "435ce60f233b6718aaa04bb7068dd641b536299b";
const char *OPENTELEMETRY_SDK_SHORT_VERSION   = "1.8.2";
const char *OPENTELEMETRY_SDK_FULL_VERSION =
    "1.8.2-NONE-NONE-37-pre_release_1.8.2-435ce60f233b6718aaa04bb7068dd641b536299b";
const char *OPENTELEMETRY_SDK_BUILD_DATE = "Tue 31 Jan 2023 04:01:10 PM UTC";
}  // namespace version
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
