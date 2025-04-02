// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#define OPENTELEMETRY_SDK_VERSION "1.20.0"

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

OPENTELEMETRY_EXPORT const int GetMajorVersion();
OPENTELEMETRY_EXPORT const int GetMinorVersion();
OPENTELEMETRY_EXPORT const int GetPatchVersion();
OPENTELEMETRY_EXPORT const char* GetPreRelease();
OPENTELEMETRY_EXPORT const char* GetBuildMetadata();
OPENTELEMETRY_EXPORT const char* GetShortVersion();
OPENTELEMETRY_EXPORT const char* GetFullVersion();
OPENTELEMETRY_EXPORT const char* GetBuildData();
}  // namespace version
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
