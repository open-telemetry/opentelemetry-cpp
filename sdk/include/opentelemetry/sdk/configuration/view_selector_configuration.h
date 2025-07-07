// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include "opentelemetry/version.h"
#include "opentelemetry/sdk/configuration/instrument_type.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

// YAML-SCHEMA: schema/meter_provider.json
// YAML-NODE: ViewSelector
class ViewSelectorConfiguration
{
public:
  std::string instrument_name;
  InstrumentType instrument_type{InstrumentType::counter};
  std::string unit;
  std::string meter_name;
  std::string meter_version;
  std::string meter_schema_url;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
