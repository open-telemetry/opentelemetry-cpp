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
const int major_version    = 1;
const int minor_version    = 13;
const int patch_version    = 0;
const char *pre_release    = "NONE";
const char *build_metadata = "NONE";
const char *short_version  = "1.13.0";
const char *full_version   = "1.13.0-NONE-NONE";
const char *build_date     = "Wed Dec  6 00:27:20 UTC 2023";

}  // namespace version
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
