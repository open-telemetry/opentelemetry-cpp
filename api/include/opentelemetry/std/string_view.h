// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/version.h"

#include "opentelemetry/std/utility.h"

#include <cstddef>
#include <memory>
#include <string_view>
#include <utility>
#include <variant>

OPENTELEMETRY_BEGIN_NAMESPACE
// Standard Type aliases in nostd namespace
namespace nostd
{

// nostd::string_view
using string_view = std::string_view;

}  // namespace nostd
OPENTELEMETRY_END_NAMESPACE
