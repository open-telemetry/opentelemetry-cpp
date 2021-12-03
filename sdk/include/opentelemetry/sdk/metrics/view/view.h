// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/view/aggregation.h"
#  include "opentelemetry/sdk/metrics/view/attributes_processor.h"
#include "opentelemetry/nostd/string_view.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
/**
 * View defines the interface to allow SDK user to
 * customize the metrics before exported.
 */

class View
{
public:
  virtual ~View() = default;

  virtual nostd::string GetName() const noexcept = 0;
  virtual nostd::string GetDescription() const noxcept = 0;
  virtual Aggregation& GetAggregation() const noexcept = 0;
  virtual AttributesProcessor& GetAttributesProcessor() const noxcept = 0; 
};
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
