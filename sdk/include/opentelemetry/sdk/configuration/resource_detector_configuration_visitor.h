// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

class ContainerResourceDetectorConfiguration;
class ExtensionResourceDetectorConfiguration;

class ResourceDetectorConfigurationVisitor
{
public:
  ResourceDetectorConfigurationVisitor()          = default;
  virtual ~ResourceDetectorConfigurationVisitor() = default;

  virtual void VisitContainer(const ContainerResourceDetectorConfiguration *model) = 0;
  virtual void VisitExtension(const ExtensionResourceDetectorConfiguration *model) = 0;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
