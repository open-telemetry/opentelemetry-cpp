#pragma once

#include "opentelemetry/metrics/instrument.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/sdk/metrics/aggregator/aggregator.h"

OPENTELEMETRY_BEGIN_NAMESPACE

namespace metrics_api = opentelemetry::metrics;

namespace sdk
{
namespace metrics
{
using AggregatorVariant = nostd::variant<nostd::shared_ptr<Aggregator<short>>,
                                         nostd::shared_ptr<Aggregator<int>>,
                                         nostd::shared_ptr<Aggregator<float>>,
                                         nostd::shared_ptr<Aggregator<double>>>;
class Record
{
public:
  explicit Record(nostd::string_view name, nostd::string_view description,
                  std::string labels, AggregatorVariant aggregator)
  {
    name_ = std::string(name);
    description_ = std::string(description);
    labels_ = labels;
    aggregator_ = aggregator;
  }

  std::string GetName() {return name_;}
  std::string GetDescription() {return description_;}
  std::string GetLabels() {return labels_;}
  AggregatorVariant GetAggregator() {return aggregator_;}

private:
  std::string name_;
  std::string description_;
  std::string labels_;
  AggregatorVariant aggregator_;
};
} // namespace metrics
} // namespace sdk
OPENTELEMETRY_END_NAMESPACE
