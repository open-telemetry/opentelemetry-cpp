// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/detail/preprocessor.h"

#define OPENTELEMETRY_SDK_VERSION "1.8.2"

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace version
{
extern const int OPENTELEMETRY_SDK_MAJOR_VERSION;
extern const int OPENTELEMETRY_SDK_MINOR_VERSION;
extern const int OPENTELEMETRY_SDK_PATCH_VERSION;
extern const char *OPENTELEMETRY_SDK_PRE_RELEASE;
extern const char *OPENTELEMETRY_SDK_BUILD_METADATA;
extern const int OPENTELEMETRY_SDK_COUNT_NEW_COMMITS;
extern const char *OPENTELEMETRY_SDK_BRANCH;
extern const char *OPENTELEMETRY_SDK_COMMIT_HASH;
extern const char *OPENTELEMETRY_SDK_SHORT_VERSION
extern const char *OPENTELEMETRY_SDK_FULL_VERSION;
// extern const char *FULL_VERSION_WITH_BRANCH_COMMITHASH;
extern const char *OPENTELEMETRY_SDK_BUILD_DATE;
}  // namespace version
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
