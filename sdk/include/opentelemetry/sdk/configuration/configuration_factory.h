// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <fstream>
#include <memory>
#include <string>

#include "opentelemetry/sdk/configuration/configuration.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

class ConfigurationFactory
{
public:
  static std::unique_ptr<Configuration> ParseFile(std::string filename);
  static std::unique_ptr<Configuration> ParseString(std::string content);
  static std::unique_ptr<Configuration> Parse(std::istream &in);
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
