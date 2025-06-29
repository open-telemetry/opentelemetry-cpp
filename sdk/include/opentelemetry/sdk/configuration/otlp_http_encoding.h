// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstdint>

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

enum enum_otlp_http_encoding : std::uint8_t
{
  protobuf,
  json
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
