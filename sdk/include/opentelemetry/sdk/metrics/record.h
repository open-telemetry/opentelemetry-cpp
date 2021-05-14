// Copyright 2021, OpenTelemetry Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once
#ifdef ENABLE_METRICS_PREVIEW

#  include <memory>
#  include "opentelemetry/metrics/instrument.h"
#  include "opentelemetry/nostd/variant.h"
#  include "opentelemetry/sdk/metrics/aggregator/aggregator.h"

OPENTELEMETRY_BEGIN_NAMESPACE

namespace metrics_api = opentelemetry::metrics;

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
