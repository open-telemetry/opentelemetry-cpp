// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

enum enum_instrument_type
{
  counter,
  histogram,
  observable_counter,
  observable_gauge,
  observable_up_down_counter,
  up_down_counter
};

class SelectorConfiguration
{
public:
  SelectorConfiguration()  = default;
  ~SelectorConfiguration() = default;

  std::string instrument_name;
  enum_instrument_type instrument_type;
  std::string unit;
  std::string meter_name;
  std::string meter_version;
  std::string meter_schema_url;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
