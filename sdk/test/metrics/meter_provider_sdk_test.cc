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
      new InstrumentSelector(InstrumentType::kCounter, "instru1", "unit1")};
  std::unique_ptr<MeterSelector> meter_selector{new MeterSelector("name1", "version1", "schema1")};

  mp1.AddView(std::move(instrument_selector), std::move(meter_selector), std::move(view));

  // cleanup properly without crash
  mp1.ForceFlush();
  mp1.Shutdown();
}

#if OPENTELEMETRY_ABI_VERSION_NO >= 2
TEST(MeterProvider, GetMeterAbiv2)
{
  MeterProvider mp;

  auto m1 = mp.GetMeter("name1", "version1", "url1");
  ASSERT_NE(nullptr, m1);

  auto m2 = mp.GetMeter("name2", "version2", "url2", nullptr);
  ASSERT_NE(nullptr, m2);

  auto m3 = mp.GetMeter("name3", "version3", "url3", {{"accept_single_attr", true}});
  ASSERT_NE(nullptr, m3);
  {
    auto meter = static_cast<opentelemetry::sdk::metrics::Meter *>(m3.get());
    auto scope = meter->GetInstrumentationScope();
    auto attrs = scope->GetAttributes();
    ASSERT_EQ(attrs.size(), 1);
    auto attr = attrs.find("accept_single_attr");
    ASSERT_FALSE(attr == attrs.end());
    ASSERT_TRUE(opentelemetry::nostd::holds_alternative<bool>(attr->second));
    EXPECT_EQ(opentelemetry::nostd::get<bool>(attr->second), true);
  }

  std::pair<opentelemetry::nostd::string_view, opentelemetry::common::AttributeValue> attr4 = {
      "accept_single_attr", true};
  auto m4 = mp.GetMeter("name4", "version4", "url4", {attr4});
  ASSERT_NE(nullptr, m4);
  {
    auto meter = static_cast<opentelemetry::sdk::metrics::Meter *>(m4.get());
    auto scope = meter->GetInstrumentationScope();
    auto attrs = scope->GetAttributes();
    ASSERT_EQ(attrs.size(), 1);
    auto attr = attrs.find("accept_single_attr");
    ASSERT_FALSE(attr == attrs.end());
    ASSERT_TRUE(opentelemetry::nostd::holds_alternative<bool>(attr->second));
    EXPECT_EQ(opentelemetry::nostd::get<bool>(attr->second), true);
  }

  auto m5 = mp.GetMeter("name5", "version5", "url5", {{"foo", "1"}, {"bar", "2"}});
  ASSERT_NE(nullptr, m5);
  {
    auto meter = static_cast<opentelemetry::sdk::metrics::Meter *>(m5.get());
    auto scope = meter->GetInstrumentationScope();
    auto attrs = scope->GetAttributes();
    ASSERT_EQ(attrs.size(), 2);
    auto attr = attrs.find("bar");
    ASSERT_FALSE(attr == attrs.end());
    ASSERT_TRUE(opentelemetry::nostd::holds_alternative<std::string>(attr->second));
    EXPECT_EQ(opentelemetry::nostd::get<std::string>(attr->second), "2");
  }

  std::initializer_list<
      std::pair<opentelemetry::nostd::string_view, opentelemetry::common::AttributeValue>>
      attrs6 = {{"foo", "1"}, {"bar", 42}};

  auto m6 = mp.GetMeter("name6", "version6", "url6", attrs6);
  ASSERT_NE(nullptr, m6);
  {
    auto meter = static_cast<opentelemetry::sdk::metrics::Meter *>(m6.get());
    auto scope = meter->GetInstrumentationScope();
    auto attrs = scope->GetAttributes();
    ASSERT_EQ(attrs.size(), 2);
    auto attr = attrs.find("bar");
    ASSERT_FALSE(attr == attrs.end());
    ASSERT_TRUE(opentelemetry::nostd::holds_alternative<int>(attr->second));
    EXPECT_EQ(opentelemetry::nostd::get<int>(attr->second), 42);
  }

  typedef std::pair<opentelemetry::nostd::string_view, opentelemetry::common::AttributeValue> KV;

  std::initializer_list<KV> attrs7 = {{"foo", 3.14}, {"bar", "2"}};
  auto m7                          = mp.GetMeter("name7", "version7", "url7", attrs7);
  ASSERT_NE(nullptr, m7);
  {
    auto meter = static_cast<opentelemetry::sdk::metrics::Meter *>(m7.get());
    auto scope = meter->GetInstrumentationScope();
    auto attrs = scope->GetAttributes();
    ASSERT_EQ(attrs.size(), 2);
    auto attr = attrs.find("foo");
    ASSERT_FALSE(attr == attrs.end());
    ASSERT_TRUE(opentelemetry::nostd::holds_alternative<double>(attr->second));
    EXPECT_EQ(opentelemetry::nostd::get<double>(attr->second), 3.14);
  }

  auto m8 = mp.GetMeter("name8", "version8", "url8",
                        {{"a", "string"},
                         {"b", false},
                         {"c", 314159},
                         {"d", (unsigned int)314159},
                         {"e", (int32_t)-20},
                         {"f", (uint32_t)20},
                         {"g", (int64_t)-20},
                         {"h", (uint64_t)20},
                         {"i", 3.1},
                         {"j", "string"}});
  ASSERT_NE(nullptr, m8);
  {
    auto meter = static_cast<opentelemetry::sdk::metrics::Meter *>(m8.get());
    auto scope = meter->GetInstrumentationScope();
    auto attrs = scope->GetAttributes();
    ASSERT_EQ(attrs.size(), 10);
    auto attr = attrs.find("e");
    ASSERT_FALSE(attr == attrs.end());
    ASSERT_TRUE(opentelemetry::nostd::holds_alternative<int32_t>(attr->second));
    EXPECT_EQ(opentelemetry::nostd::get<int32_t>(attr->second), -20);
  }

  std::map<std::string, opentelemetry::common::AttributeValue> attr9{
      {"a", "string"},     {"b", false},        {"c", 314159},       {"d", (unsigned int)314159},
      {"e", (int32_t)-20}, {"f", (uint32_t)20}, {"g", (int64_t)-20}, {"h", (uint64_t)20},
      {"i", 3.1},          {"j", "string"}};

  auto m9 = mp.GetMeter("name9", "version9", "url9", attr9);
  ASSERT_NE(nullptr, m9);
  {
    auto meter = static_cast<opentelemetry::sdk::metrics::Meter *>(m9.get());
    auto scope = meter->GetInstrumentationScope();
    auto attrs = scope->GetAttributes();
    ASSERT_EQ(attrs.size(), 10);
    auto attr = attrs.find("h");
    ASSERT_FALSE(attr == attrs.end());
    ASSERT_TRUE(opentelemetry::nostd::holds_alternative<uint64_t>(attr->second));
    EXPECT_EQ(opentelemetry::nostd::get<uint64_t>(attr->second), 20);
  }

  // cleanup properly without crash
  mp.ForceFlush();
  mp.Shutdown();
}
#endif /* OPENTELEMETRY_ABI_VERSION_NO >= 2 */

#if OPENTELEMETRY_ABI_VERSION_NO >= 2
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
#endif /* OPENTELEMETRY_ABI_VERSION_NO >= 2 */
