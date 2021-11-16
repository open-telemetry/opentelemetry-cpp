// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifdef ENABLE_METRICS_PREVIEW

#  include <memory>
#  include "opentelemetry/_metrics/instrument.h"
#  include "opentelemetry/nostd/variant.h"
#  include "opentelemetry/sdk/_metrics/aggregator/aggregator.h"

OPENTELEMETRY_BEGIN_NAMESPACE

namespace sdk
{
namespace metrics
{
using AggregatorVariant = nostd::variant<std::shared_ptr<Aggregator<short>>,
                                         std::shared_ptr<Aggregator<int>>,
                                         std::shared_ptr<Aggregator<float>>,
                                         std::shared_ptr<Aggregator<double>>>;
class Record
{
public:
  explicit Record(nostd::string_view name,
                  nostd::string_view description,
                  std::string labels,
                  AggregatorVariant aggregator)
  {
    name_        = std::string(name);
    description_ = std::string(description);
    labels_      = labels;
    aggregator_  = aggregator;
  }

  std::string GetName() { return name_; }
  std::string GetDescription() { return description_; }
  std::string GetLabels() { return labels_; }
  AggregatorVariant GetAggregator() { return aggregator_; }

private:
  std::string name_;
  std::string description_;
  std::string labels_;
  AggregatorVariant aggregator_;
};
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
