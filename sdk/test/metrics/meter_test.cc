// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/meter.h"
#  include "opentelemetry/sdk/metrics/data/point_data.h"
#  include "opentelemetry/sdk/metrics/meter_context.h"
#  include "opentelemetry/sdk/metrics/meter_provider.h"
#  include "opentelemetry/sdk/metrics/metric_reader.h"

#  include <gtest/gtest.h>

using namespace opentelemetry;
using namespace opentelemetry::sdk::instrumentationscope;
using namespace opentelemetry::sdk::metrics;

class MockMetricReader : public MetricReader
{
public:
  MockMetricReader() = default;
  AggregationTemporality GetAggregationTemporality(
      InstrumentType instrument_type) const noexcept override
  {
    return AggregationTemporality::kCumulative;
  }
  bool OnShutDown(std::chrono::microseconds timeout) noexcept override { return true; }
  bool OnForceFlush(std::chrono::microseconds timeout) noexcept override { return true; }
  void OnInitialized() noexcept override {}
};

namespace
{
nostd::shared_ptr<metrics::Meter> InitMeter(MetricReader **metricReaderPtr)
{
  static std::shared_ptr<metrics::MeterProvider> provider(new MeterProvider());
  std::unique_ptr<MetricReader> metric_reader(new MockMetricReader());
  *metricReaderPtr = metric_reader.get();
  auto p           = std::static_pointer_cast<MeterProvider>(provider);
  p->AddMetricReader(std::move(metric_reader));
  auto meter = provider->GetMeter("meter_name");
  return meter;
}
}  // namespace

void asyc_generate_measurements(opentelemetry::metrics::ObserverResult observer, void *state)
{
  auto observer_long =
      nostd::get<nostd::shared_ptr<opentelemetry::metrics::ObserverResultT<long>>>(observer);
  observer_long->Observe(10l);
}

TEST(MeterTest, BasicAsyncTests)
{
  MetricReader *metric_reader_ptr = nullptr;
  auto meter                      = InitMeter(&metric_reader_ptr);
  auto observable_counter         = meter->CreateLongObservableCounter("observable_counter");
  observable_counter->AddCallback(asyc_generate_measurements, nullptr);

  size_t count = 0;
  metric_reader_ptr->Collect([&count](ResourceMetrics &metric_data) {
    EXPECT_EQ(metric_data.scope_metric_data_.size(), 1);
    if (metric_data.scope_metric_data_.size())
    {
      EXPECT_EQ(metric_data.scope_metric_data_[0].metric_data_.size(), 1);
      if (metric_data.scope_metric_data_.size())
      {
        count += metric_data.scope_metric_data_[0].metric_data_.size();
        EXPECT_EQ(count, 1);
      }
    }
    return true;
  });
}

#endif
