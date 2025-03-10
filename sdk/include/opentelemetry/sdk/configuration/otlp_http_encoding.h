// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

enum enum_otlp_http_encoding
{
  protobuf,
  json
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
