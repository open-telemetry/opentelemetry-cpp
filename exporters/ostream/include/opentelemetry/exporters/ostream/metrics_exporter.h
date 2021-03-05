#pragma once

#include <iostream>
#include <string>
#include "opentelemetry/sdk/metrics/aggregator/exact_aggregator.h"
#include "opentelemetry/sdk/metrics/aggregator/gauge_aggregator.h"
#include "opentelemetry/sdk/metrics/aggregator/histogram_aggregator.h"
#include "opentelemetry/sdk/metrics/exporter.h"
#include "opentelemetry/sdk/metrics/record.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdkmetrics = opentelemetry::sdk::metrics;

namespace exporter
{
namespace metrics
{

/**
 * The OStreamMetricsExporter exports record data through an ostream
 */
class OStreamMetricsExporter final : public sdkmetrics::MetricsExporter
{
public:
  /**
   * Create an OStreamMetricsExporter. This constructor takes in a reference to an ostream that the
   * export() function will send span data into.
   * The default ostream is set to stdout
   */
  explicit OStreamMetricsExporter(std::ostream &sout = std::cout) noexcept;

  sdkmetrics::ExportResult Export(const std::vector<sdkmetrics::Record> &records) noexcept override;

private:
  std::ostream &sout_;

  /**
   * Send specific data from the given AggregatorVariant based on what AggregatorKind
   * it is holding. Each Aggregator holds data differently, so each have their own
   * custom printing.
   */
  template <typename T>
  void PrintAggregatorVariant(sdkmetrics::AggregatorVariant value)
  {
    auto agg     = nostd::get<std::shared_ptr<sdkmetrics::Aggregator<T>>>(value);
    auto aggKind = agg->get_aggregator_kind();

    if (!agg)
      return;
    switch (aggKind)
    {
      case sdkmetrics::AggregatorKind::Counter: {
        sout_ << "\n  sum         : " << agg->get_checkpoint()[0];
      }
      break;
      case sdkmetrics::AggregatorKind::MinMaxSumCount: {
        auto mmsc = agg->get_checkpoint();
        sout_ << "\n  min         : " << mmsc[0] << "\n  max         : " << mmsc[1]
              << "\n  sum         : " << mmsc[2] << "\n  count       : " << mmsc[3];
      }
      break;
      case sdkmetrics::AggregatorKind::Gauge: {
        auto timestamp = agg->get_checkpoint_timestamp();

        sout_ << "\n  last value  : " << agg->get_checkpoint()[0]
              << "\n  timestamp   : " << std::to_string(timestamp.time_since_epoch().count());
      }
      break;
      case sdkmetrics::AggregatorKind::Exact: {
        // TODO: Find better way to print quantiles
        if (agg->get_quant_estimation())
        {
          sout_ << "\n  quantiles   : "
                << "[0: " << agg->get_quantiles(0) << ", "
                << ".25: " << agg->get_quantiles(.25) << ", "
                << ".50: " << agg->get_quantiles(.50) << ", "
                << ".75: " << agg->get_quantiles(.75) << ", "
                << "1: " << agg->get_quantiles(1) << ']';
        }
        else
        {
          auto vec    = agg->get_checkpoint();
          size_t size = vec.size();
          size_t i    = 1;

          sout_ << "\n  values      : " << '[';

          for (auto val : vec)
          {
            sout_ << val;
            if (i != size)
              sout_ << ", ";
            i++;
          }
          sout_ << ']';
        }
      }
      break;
      case sdkmetrics::AggregatorKind::Histogram: {
        auto boundaries = agg->get_boundaries();
        auto counts     = agg->get_counts();

        size_t boundaries_size = boundaries.size();
        size_t counts_size     = counts.size();

        sout_ << "\n  buckets     : " << '[';

        for (size_t i = 0; i < boundaries_size; i++)
        {
          sout_ << boundaries[i];

          if (i != boundaries_size - 1)
            sout_ << ", ";
        }
        sout_ << ']';

        sout_ << "\n  counts      : " << '[';
        for (size_t i = 0; i < counts_size; i++)
        {
          sout_ << counts[i];

          if (i != counts_size - 1)
            sout_ << ", ";
        }
        sout_ << ']';
      }
      break;
      case sdkmetrics::AggregatorKind::Sketch: {
        auto boundaries = agg->get_boundaries();
        auto counts     = agg->get_counts();

        size_t boundaries_size = boundaries.size();
        size_t counts_size     = counts.size();

        sout_ << "\n  buckets     : " << '[';

        for (size_t i = 0; i < boundaries_size; i++)
        {
          sout_ << boundaries[i];

          if (i != boundaries_size - 1)
            sout_ << ", ";
        }
        sout_ << ']';

        sout_ << "\n  counts      : " << '[';
        for (size_t i = 0; i < counts_size; i++)
        {
          sout_ << counts[i];

          if (i != counts_size - 1)
            sout_ << ", ";
        }
        sout_ << ']';
      }
      break;
    }
  }
};
}  // namespace metrics
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
