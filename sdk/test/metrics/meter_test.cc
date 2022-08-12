// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/meter.h"
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
  virtual bool OnShutDown(std::chrono::microseconds timeout) noexcept override { return true; }
  virtual bool OnForceFlush(std::chrono::microseconds timeout) noexcept override { return true; }
  virtual void OnInitialized() noexcept override {}
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
  // auto sdk_meter = std::static_pointer_cast<opentelemetry::sdk::metrics::Meter>(meter);

  /*
    provider->
    auto context = std::make_shared<opentelemetry::sdk::metrics::MeterContext>();
    std::unique_ptr<MetricReader> metric_reader(new MockMetricReader());
    *metricReaderPtr = metric_reader.get();
    context->AddMetricReader(std::move(metric_reader));
    std::shared_ptr<Meter> meter(new Meter(context));
    return meter;
    */
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
  MetricReader *metricReaderPtr = nullptr;
  auto meter                    = InitMeter(&metricReaderPtr);
  auto observable_counter       = meter->CreateLongObservableCounter("observable_counter");
  observable_counter->AddCallback(asyc_generate_measurements, nullptr);

  size_t count = 0;
  metricReaderPtr->Collect([&count](ResourceMetrics &metric_data) {
    count += metric_data.scope_metric_data_.size();
    EXPECT_EQ(count, 1);
    return true;
  });
}

#endif
