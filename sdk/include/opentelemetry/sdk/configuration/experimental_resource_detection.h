// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>
#include <vector>

#include "opentelemetry/sdk/configuration/include_exclude_configuration.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

// YAML-SCHEMA: schema/resource.json
// YAML-NODE: ExperimentalContainerResourceDetector
class ExperimentalContainerResourceDetector
{};

// YAML-SCHEMA: schema/resource.json
// YAML-NODE: ExperimentalHostResourceDetector
class ExperimentalHostResourceDetector
{};

// YAML-SCHEMA: schema/resource.json
// YAML-NODE: ExperimentalProcessResourceDetector
class ExperimentalProcessResourceDetector
{};

// YAML-SCHEMA: schema/resource.json
// YAML-NODE: ExperimentalServiceResourceDetector
class ExperimentalServiceResourceDetector
{};

// YAML-SCHEMA: schema/resource.json
// YAML-NODE: ExperimentalResourceDetector
class ExperimentalResourceDetector
{
public:
  std::unique_ptr<ExperimentalContainerResourceDetector> container;
  std::unique_ptr<ExperimentalHostResourceDetector> host;
  std::unique_ptr<ExperimentalProcessResourceDetector> process;
  std::unique_ptr<ExperimentalServiceResourceDetector> service;
};

// YAML-SCHEMA: schema/resource.json
// YAML-NODE: ExperimentalResourceDetection
class ExperimentalResourceDetection
{
public:
  std::unique_ptr<IncludeExcludeConfiguration> attributes;
  std::vector<std::unique_ptr<ExperimentalResourceDetector>> detectors;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
