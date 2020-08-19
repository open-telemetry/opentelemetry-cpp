#include "opentelemetry/sdk/metrics/ungrouped_processor.h"

#define UNGROUPED_PROCESSOR_STRINGER(x) (#x)

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
std::vector<sdkmetrics::Record> UngroupedMetricsProcessor::CheckpointSelf() noexcept
{
  std::vector<sdkmetrics::Record> metric_records;

  for (auto iter : batch_map_)
  {
    /**
     * TODO: micro-optimization, scan once or change to using a struct with custom hash function,
     * to hold the data.
     */

    std::string key               = iter.first;
    std::size_t description_index = key.find("/description/");
    std::size_t labels_index      = key.find("/labels/");
    std::size_t instrument_index  = key.find("/instrument/");

    std::string name = key.substr(6, description_index - 6);
    std::string description =
        key.substr(description_index + 13, labels_index - description_index - 13);
    std::string labels = key.substr(labels_index + 8, instrument_index - labels_index - 8);

    sdkmetrics::Record r{name, description, labels, iter.second};

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

void UngroupedMetricsProcessor::process(sdkmetrics::Record record) noexcept
{
  auto aggregator         = record.GetAggregator();
  std::string label       = record.GetLabels();
  std::string name        = record.GetName();
  std::string description = record.GetDescription();
  std::string instrument  = UNGROUPED_PROCESSOR_STRINGER(get_instrument(aggregator));

  std::string batch_key = "/name/" + name + "/description/" + description + "/labels/" + label +
                          "/instrument/" + instrument;

  /**
   * If we have already seen this aggregator then we will merge it with the copy that exists in the
   *batch_map_ The call to merge here combines only identical records (same key)
   **/
  if (batch_map_.find(batch_key) != batch_map_.end())
  {
    auto batch_value = batch_map_[batch_key];

    if (nostd::holds_alternative<std::shared_ptr<sdkmetrics::Aggregator<short>>>(aggregator))
    {
      auto batch_value_reference_short =
          nostd::get<std::shared_ptr<sdkmetrics::Aggregator<short>>>(batch_value);
      auto aggregator_reference_short =
          nostd::get<std::shared_ptr<sdkmetrics::Aggregator<short>>>(aggregator);

      merge_aggregators<short>(batch_value_reference_short, aggregator_reference_short);
    }
    else if (nostd::holds_alternative<std::shared_ptr<sdkmetrics::Aggregator<int>>>(aggregator))
    {
      auto batch_value_reference_int =
          nostd::get<std::shared_ptr<sdkmetrics::Aggregator<int>>>(batch_value);
      auto aggregator_reference_int =
          nostd::get<std::shared_ptr<sdkmetrics::Aggregator<int>>>(aggregator);

      merge_aggregators<int>(batch_value_reference_int, aggregator_reference_int);
    }
    else if (nostd::holds_alternative<std::shared_ptr<sdkmetrics::Aggregator<float>>>(aggregator))
    {
      auto batch_value_reference_float =
          nostd::get<std::shared_ptr<sdkmetrics::Aggregator<float>>>(batch_value);
      auto aggregator_reference_float =
          nostd::get<std::shared_ptr<sdkmetrics::Aggregator<float>>>(aggregator);

      merge_aggregators<float>(batch_value_reference_float, aggregator_reference_float);
    }
    else if (nostd::holds_alternative<std::shared_ptr<sdkmetrics::Aggregator<double>>>(aggregator))
    {
      auto batch_value_reference_double =
          nostd::get<std::shared_ptr<sdkmetrics::Aggregator<double>>>(batch_value);
      auto aggregator_reference_double =
          nostd::get<std::shared_ptr<sdkmetrics::Aggregator<double>>>(aggregator);

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
    if (nostd::holds_alternative<std::shared_ptr<sdkmetrics::Aggregator<short>>>(aggregator))
    {
      auto record_agg_short =
          nostd::get<std::shared_ptr<sdkmetrics::Aggregator<short>>>(aggregator);
      auto aggregator_short = aggregator_copy<short>(record_agg_short);

      merge_aggregators<short>(aggregator_short, record_agg_short);

      batch_map_[batch_key] = aggregator_short;
    }
    else if (nostd::holds_alternative<std::shared_ptr<sdkmetrics::Aggregator<int>>>(aggregator))
    {
      auto record_agg_int = nostd::get<std::shared_ptr<sdkmetrics::Aggregator<int>>>(aggregator);
      auto aggregator_int = aggregator_copy<int>(record_agg_int);

      merge_aggregators<int>(aggregator_int, record_agg_int);

      batch_map_[batch_key] = aggregator_int;
    }
    else if (nostd::holds_alternative<std::shared_ptr<sdkmetrics::Aggregator<float>>>(aggregator))
    {
      auto record_agg_float =
          nostd::get<std::shared_ptr<sdkmetrics::Aggregator<float>>>(aggregator);
      auto aggregator_float = aggregator_copy<float>(record_agg_float);

      merge_aggregators<float>(aggregator_float, record_agg_float);

      batch_map_[batch_key] = aggregator_float;
    }
    else if (nostd::holds_alternative<std::shared_ptr<sdkmetrics::Aggregator<double>>>(aggregator))
    {
      auto record_agg_double =
          nostd::get<std::shared_ptr<sdkmetrics::Aggregator<double>>>(aggregator);
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

#undef UNGROUPED_PROCESSOR_STRINGER
