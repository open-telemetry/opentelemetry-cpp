// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <fstream>
#include <memory>
#include <string>

#include "opentelemetry/sdk/configuration/configuration.h"
#include "opentelemetry/sdk/configuration/document_node.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

class ConfigurationFactory
{
public:
  static std::unique_ptr<Configuration> ParseConfiguration(std::unique_ptr<Document> doc);
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
