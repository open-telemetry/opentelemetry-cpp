// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace common
{

bool OPENTELEMETRY_EXPORT GetSdkDisabled();

}  // namespace common
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
