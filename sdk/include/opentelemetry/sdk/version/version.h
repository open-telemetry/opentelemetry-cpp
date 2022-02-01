// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/detail/preprocessor.h"

#define OPENTELEMETRY_SDK_VERSION "1.2.0"

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace version
{
extern const int MAJOR_VERSION;
extern const int MINOR_VERSION;
extern const int PATCH_VERSION;
extern const char *PRE_RELEASE;
extern const char *BUILD_METADATA;
extern const int COUNT_NEW_COMMITS;
extern const char *BRANCH;
extern const char *COMMIT_HASH;
extern const char *FULL_VERSION;
extern const char *FULL_VERSION_WITH_BRANCH_COMMITHASH;
extern const char *BUILD_DATE;
}  // namespace version
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
