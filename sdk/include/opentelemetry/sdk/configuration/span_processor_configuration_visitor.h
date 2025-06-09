// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

class BatchSpanProcessorConfiguration;
class SimpleSpanProcessorConfiguration;
class ExtensionSpanProcessorConfiguration;

class SpanProcessorConfigurationVisitor
{
public:
  SpanProcessorConfigurationVisitor()          = default;
  virtual ~SpanProcessorConfigurationVisitor() = default;

  virtual void VisitBatch(const BatchSpanProcessorConfiguration *model)         = 0;
  virtual void VisitSimple(const SimpleSpanProcessorConfiguration *model)       = 0;
  virtual void VisitExtension(const ExtensionSpanProcessorConfiguration *model) = 0;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
