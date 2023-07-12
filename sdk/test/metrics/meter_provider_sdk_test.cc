// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "common.h"

#include <gtest/gtest.h>
#include "opentelemetry/sdk/metrics/export/metric_producer.h"
#include "opentelemetry/sdk/metrics/meter.h"
#include "opentelemetry/sdk/metrics/meter_provider.h"
#include "opentelemetry/sdk/metrics/metric_reader.h"
#include "opentelemetry/sdk/metrics/push_metric_exporter.h"
#include "opentelemetry/sdk/metrics/view/instrument_selector.h"
#include "opentelemetry/sdk/metrics/view/meter_selector.h"

using namespace opentelemetry::sdk::metrics;

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

  // Should be an sdk::metrics::Meter
#ifdef OPENTELEMETRY_RTTI_ENABLED
  auto sdkMeter1 = dynamic_cast<Meter *>(m1.get());
#else
  auto sdkMeter1 = static_cast<Meter *>(m1.get());
#endif
  ASSERT_NE(nullptr, sdkMeter1);
  std::unique_ptr<MockMetricExporter> exporter(new MockMetricExporter());
  std::unique_ptr<MetricReader> reader{new MockMetricReader(std::move(exporter))};
  mp1.AddMetricReader(std::move(reader));

  std::unique_ptr<View> view{std::unique_ptr<View>()};
  std::unique_ptr<InstrumentSelector> instrument_selector{
      new InstrumentSelector(InstrumentType::kCounter, "instru1")};
  std::unique_ptr<MeterSelector> meter_selector{new MeterSelector("name1", "version1", "schema1")};

  mp1.AddView(std::move(instrument_selector), std::move(meter_selector), std::move(view));

  // cleanup properly without crash
  mp1.ForceFlush();
  mp1.Shutdown();
}

#ifdef ENABLE_REMOVE_METER_PREVIEW
TEST(MeterProvider, RemoveMeter)
{
  MeterProvider mp;

  auto m1 = mp.GetMeter("test", "1", "URL");
  ASSERT_NE(nullptr, m1);

  // Will return the same meter
  auto m2 = mp.GetMeter("test", "1", "URL");
  ASSERT_NE(nullptr, m2);
  ASSERT_EQ(m1, m2);

  mp.RemoveMeter("unknown", "0", "");

  // Will decrease use_count() on m1 and m2
  mp.RemoveMeter("test", "1", "URL");

  // Will create a different meter
  auto m3 = mp.GetMeter("test", "1", "URL");
  ASSERT_NE(nullptr, m3);
  ASSERT_NE(m1, m3);
  ASSERT_NE(m2, m3);

  // Will decrease use_count() on m3
  mp.RemoveMeter("test", "1", "URL");

  // Will do nothing
  mp.RemoveMeter("test", "1", "URL");

  // cleanup properly without crash
  mp.ForceFlush();
  mp.Shutdown();
}
#endif
