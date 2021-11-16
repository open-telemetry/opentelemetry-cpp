// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/_metrics/ungrouped_processor.h"

OPENTELEMETRY_BEGIN_NAMESPACE

namespace sdk
{

namespace metrics
{

UngroupedMetricsProcessor::UngroupedMetricsProcessor(bool stateful)
{
  stateful_ = stateful;
}

/**
 * CheckpointSelf will return a vector of records to be exported. This function will go through
 *aggregators that have been sent through process() and return them as a vector of records.
 **/
std::vector<opentelemetry::sdk::metrics::Record>
UngroupedMetricsProcessor::CheckpointSelf() noexcept
{
  std::vector<opentelemetry::sdk::metrics::Record> metric_records;

  for (auto iter : batch_map_)
  {
    // Create a record from the held KeyStruct values and add to the Checkpoint
    KeyStruct key = iter.first;
    opentelemetry::sdk::metrics::Record r{key.name, key.description, key.labels, iter.second};

    metric_records.push_back(r);
  }

  return metric_records;
}

/**
 * Once Process is called, FinishCollection() should also be called. In the case of a non stateful
 *processor the map will be reset.
 **/
void UngroupedMetricsProcessor::FinishedCollection() noexcept
{
  if (!stateful_)
  {
    batch_map_ = {};
  }
}

void UngroupedMetricsProcessor::process(opentelemetry::sdk::metrics::Record record) noexcept
{
  auto aggregator         = record.GetAggregator();
  std::string label       = record.GetLabels();
  std::string name        = record.GetName();
  std::string description = record.GetDescription();

  KeyStruct batch_key = KeyStruct(name, description, label, get_instrument(aggregator));

  /**
   * If we have already seen this aggregator then we will merge it with the copy that exists in the
   *batch_map_ The call to merge here combines only identical records (same key)
   **/
  if (batch_map_.find(batch_key) != batch_map_.end())
  {
    auto batch_value = batch_map_[batch_key];

    if (nostd::holds_alternative<std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<short>>>(
            aggregator))
    {
      auto batch_value_reference_short =
          nostd::get<std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<short>>>(batch_value);
      auto aggregator_reference_short =
          nostd::get<std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<short>>>(aggregator);

      merge_aggregators<short>(batch_value_reference_short, aggregator_reference_short);
    }
    else if (nostd::holds_alternative<
                 std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<int>>>(aggregator))
    {
      auto batch_value_reference_int =
          nostd::get<std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<int>>>(batch_value);
      auto aggregator_reference_int =
          nostd::get<std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<int>>>(aggregator);

      merge_aggregators<int>(batch_value_reference_int, aggregator_reference_int);
    }
    else if (nostd::holds_alternative<
                 std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<float>>>(aggregator))
    {
      auto batch_value_reference_float =
          nostd::get<std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<float>>>(batch_value);
      auto aggregator_reference_float =
          nostd::get<std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<float>>>(aggregator);

      merge_aggregators<float>(batch_value_reference_float, aggregator_reference_float);
    }
    else if (nostd::holds_alternative<
                 std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<double>>>(aggregator))
    {
      auto batch_value_reference_double =
          nostd::get<std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<double>>>(batch_value);
      auto aggregator_reference_double =
          nostd::get<std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<double>>>(aggregator);

      merge_aggregators<double>(batch_value_reference_double, aggregator_reference_double);
    }
    return;
  }
  /**
   * If the processor is stateful and this aggregator has not be seen by the processor yet.
   * We create a copy of this aggregator, merge it with the aggregator from the given record.
   * Then set this copied aggregator with the batch_key in the batch_map_
   **/
  if (stateful_)
  {
    if (nostd::holds_alternative<std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<short>>>(
            aggregator))
    {
      auto record_agg_short =
          nostd::get<std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<short>>>(aggregator);
      auto aggregator_short = aggregator_copy<short>(record_agg_short);

      merge_aggregators<short>(aggregator_short, record_agg_short);

      batch_map_[batch_key] = aggregator_short;
    }
    else if (nostd::holds_alternative<
                 std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<int>>>(aggregator))
    {
      auto record_agg_int =
          nostd::get<std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<int>>>(aggregator);
      auto aggregator_int = aggregator_copy<int>(record_agg_int);

      merge_aggregators<int>(aggregator_int, record_agg_int);

      batch_map_[batch_key] = aggregator_int;
    }
    else if (nostd::holds_alternative<
                 std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<float>>>(aggregator))
    {
      auto record_agg_float =
          nostd::get<std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<float>>>(aggregator);
      auto aggregator_float = aggregator_copy<float>(record_agg_float);

      merge_aggregators<float>(aggregator_float, record_agg_float);

      batch_map_[batch_key] = aggregator_float;
    }
    else if (nostd::holds_alternative<
                 std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<double>>>(aggregator))
    {
      auto record_agg_double =
          nostd::get<std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<double>>>(aggregator);
      auto aggregator_double = aggregator_copy<double>(record_agg_double);

      merge_aggregators<double>(aggregator_double, record_agg_double);

      batch_map_[batch_key] = aggregator_double;
    }
  }
  else
  {
    /**
     * If the processor is not stateful, we don't need to create a copy of the aggregator, since the
     *map will be reset from FinishedCollection().
     **/
    batch_map_[batch_key] = aggregator;
  }
}

}  // namespace metrics
}  // namespace sdk

OPENTELEMETRY_END_NAMESPACE
#endif
