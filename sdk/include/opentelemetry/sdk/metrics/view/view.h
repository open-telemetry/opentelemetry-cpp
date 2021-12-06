// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/nostd/string_view.h"
#  include "opentelemetry/sdk/metrics/view/aggregation.h"
#  include "opentelemetry/sdk/metrics/view/attributes_processor.h"

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

  virtual std::string GetName() const noexcept                                            = 0;
  virtual std::string GetDescription() const noexcept                                     = 0;
  virtual const opentelemetry::sdk::metrics::Aggregation &GetAggregation() const noexcept = 0;
  virtual const opentelemetry::sdk::metrics::AttributesProcessor &GetAttributesProcessor()
      const noexcept = 0;
};
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
