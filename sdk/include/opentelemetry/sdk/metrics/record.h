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
using AggregatorVariant = nostd::variant<std::unique_ptr<Aggregator<short>>,
                                         std::unique_ptr<Aggregator<int>>,
                                         std::unique_ptr<Aggregator<float>>,
                                         std::unique_ptr<Aggregator<double>>>;
class Record
{
public:
  explicit Record(std::string name, std::string description,
                  std::string labels,
                  AggregatorVariant aggregator)
  {
    name_ = name;
    description_ = description;
    labels_ = labels;
    aggregator_ = std::move(aggregator);
  }

  std::string GetName() {return name_;}
  std::string GetDescription() {return description_;}
  std::string GetLabels() {return labels_;}
  AggregatorVariant GetAggregator() {return std::move(aggregator_);}

private:
  std::string name_;
  std::string description_;
  std::string labels_;
  AggregatorVariant aggregator_;
};
} // namespace metrics
} // namespace sdk
OPENTELEMETRY_END_NAMESPACE