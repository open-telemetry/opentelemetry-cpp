// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include <gtest/gtest.h>
#  include "opentelemetry/sdk/metrics/export/metric_producer.h"
#  include "opentelemetry/sdk/metrics/meter.h"
#  include "opentelemetry/sdk/metrics/meter_provider.h"
#  include "opentelemetry/sdk/metrics/metric_exporter.h"
#  include "opentelemetry/sdk/metrics/metric_reader.h"
#  include "opentelemetry/sdk/metrics/view/instrument_selector.h"
#  include "opentelemetry/sdk/metrics/view/meter_selector.h"

using namespace opentelemetry::sdk::metrics;

class MockMetricExporter : public MetricExporter
{

public:
  MockMetricExporter() = default;
  opentelemetry::sdk::common::ExportResult Export(const ResourceMetrics &records) noexcept override
  {
    return opentelemetry::sdk::common::ExportResult::kSuccess;
  }

  bool ForceFlush(
      std::chrono::microseconds timeout = (std::chrono::microseconds::max)()) noexcept override
  {
    return true;
  }

  bool Shutdown(std::chrono::microseconds timeout = std::chrono::microseconds(0)) noexcept override
  {
    return true;
  }
};

class MockMetricReader : public MetricReader
{
public:
  MockMetricReader(std::unique_ptr<MetricExporter> exporter) : exporter_(std::move(exporter)) {}
  virtual bool OnForceFlush(std::chrono::microseconds timeout) noexcept override { return true; }
  virtual bool OnShutDown(std::chrono::microseconds timeout) noexcept override { return true; }
  virtual void OnInitialized() noexcept override {}

private:
  std::unique_ptr<MetricExporter> exporter_;
};

TEST(MeterProvider, GetMeter)
{

  MeterProvider mp1;
  //   std::unique_ptr<View> view{std::unique_ptr<View>()};
  // MeterProvider mp1(std::move(exporters), std::move(readers), std::move(views);
  auto m1 = mp1.GetMeter("test");
  auto m2 = mp1.GetMeter("test");
  auto m3 = mp1.GetMeter("different", "1.0.0");
  auto m4 = mp1.GetMeter("");
  auto m5 = mp1.GetMeter(opentelemetry::nostd::string_view{});
  auto m6 = mp1.GetMeter("different", "1.0.0", "https://opentelemetry.io/schemas/1.2.0");
  ASSERT_NE(nullptr, m1);
  ASSERT_NE(nullptr, m2);
  ASSERT_NE(nullptr, m3);
  ASSERT_NE(nullptr, m6);

  // Should return the same instance each time.
  ASSERT_EQ(m1, m2);
  ASSERT_NE(m1, m3);
  ASSERT_EQ(m4, m5);
  ASSERT_NE(m3, m6);

  // Should be an sdk::trace::Tracer with the processor attached.
#  ifdef OPENTELEMETRY_RTTI_ENABLED
  auto sdkMeter1 = dynamic_cast<Meter *>(m1.get());
#  else
  auto sdkMeter1 = static_cast<Meter *>(m1.get());
#  endif
  ASSERT_NE(nullptr, sdkMeter1);
  std::unique_ptr<MockMetricExporter> exporter(new MockMetricExporter());
  std::unique_ptr<MetricReader> reader{new MockMetricReader(std::move(exporter))};
  ASSERT_NO_THROW(mp1.AddMetricReader(std::move(reader)));

  std::unique_ptr<View> view{std::unique_ptr<View>()};
  std::unique_ptr<InstrumentSelector> instrument_selector{
      new InstrumentSelector(InstrumentType::kCounter, "instru1")};
  std::unique_ptr<MeterSelector> meter_selector{new MeterSelector("name1", "version1", "schema1")};
  ASSERT_NO_THROW(
      mp1.AddView(std::move(instrument_selector), std::move(meter_selector), std::move(view)));
}
#endif
