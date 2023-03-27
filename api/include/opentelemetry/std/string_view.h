// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/version.h"

#include <string_view>

OPENTELEMETRY_BEGIN_NAMESPACE
// Standard Type aliases in nostd namespace
namespace nostd
{

// nostd::string_view
using string_view = std::string_view;

}  // namespace nostd
OPENTELEMETRY_END_NAMESPACE
