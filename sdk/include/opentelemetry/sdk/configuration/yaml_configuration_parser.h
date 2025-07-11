// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>
#include <string>

#include "opentelemetry/version.h"
#include "opentelemetry/sdk/configuration/configuration.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

class OPENTELEMETRY_EXPORT_TYPE YamlConfigurationParser
{
public:
  static std::unique_ptr<Configuration> ParseFile(const std::string &filename);
  static std::unique_ptr<Configuration> ParseString(const std::string &source,
                                                    const std::string &content);
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
