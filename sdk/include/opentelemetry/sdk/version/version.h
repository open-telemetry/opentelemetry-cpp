// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/detail/preprocessor.h"

#define OPENTELEMETRY_SDK_ABI_VERSION_NO 0
#define OPENTELEMETRY_SDK_VERSION "0.1.0"
#define OPENTELEMETRY_SDK_ABI_VERSION OPENTELEMETRY_STRINGIFY(OPENTELEMETRY_SDK_ABI_VERSION_NO)

// clang-format off
#define OPENTELEMETRY_SDK_BEGIN_NAMESPACE \
  namespace opentelemetry { namespace sdk { inline namespace OPENTELEMETRY_CONCAT(v, OPENTELEMETRY_SDK_ABI_VERSION_NO) {

#define OPENTELEMETRY_SDK_END_NAMESPACE \
  }}}

#define OPENTELEMETRY_SDK_NAMESPACE opentelemetry :: sdk :: OPENTELEMETRY_CONCAT(v, OPENTELEMETRY_SDK_ABI_VERSION_NO)

// clang-format on

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
