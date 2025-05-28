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

// YAML-SCHEMA: schema/meter_provider.json
// YAML-NODE: ViewSelector
class ViewSelectorConfiguration
{
public:
  ViewSelectorConfiguration()  = default;
  ~ViewSelectorConfiguration() = default;

  std::string instrument_name;
  enum_instrument_type instrument_type{counter};
  std::string unit;
  std::string meter_name;
  std::string meter_version;
  std::string meter_schema_url;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
