// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/detail/preprocessor.h"

#define OPENTELEMETRY_SDK_VERSION "1.13.0"

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace version
{
extern const int major_version;
extern const int minor_version;
extern const int patch_version;
extern const char *pre_release;
extern const char *build_metadata;
extern const char *short_version;
extern const char *full_version;
extern const char *build_date;
}  // namespace version
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
