// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <prometheus/metric_family.h>
#include <prometheus/metric_type.h>
#include <string>
#include <utility>
#include <vector>

#include "opentelemetry/common/timestamp.h"
#include "opentelemetry/exporters/prometheus/exporter_utils.h"
#include "opentelemetry/nostd/unique_ptr.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/sdk/instrumentationscope/instrumentation_scope.h"
#include "opentelemetry/sdk/metrics/data/metric_data.h"
#include "opentelemetry/sdk/metrics/data/point_data.h"
#include "opentelemetry/sdk/metrics/export/metric_producer.h"
#include "opentelemetry/sdk/metrics/instruments.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/version.h"
#include "prometheus_test_helper.h"

using namespace prometheus_test;
using opentelemetry::exporter::metrics::PrometheusExporterUtils;
namespace metric_sdk        = opentelemetry::sdk::metrics;
namespace prometheus_client = ::prometheus;

OPENTELEMETRY_BEGIN_NAMESPACE

namespace exporter
{
namespace metrics
{
class SanitizeNameTester
{
public:
  static std::string sanitize(std::string name)
  {
    return PrometheusExporterUtils::SanitizeNames(std::move(name));
  }
  static std::string getPrometheusUnit(const std::string &unit_abbreviation)
  {
    return PrometheusExporterUtils::GetPrometheusUnit(unit_abbreviation);
  }
  static std::string getPrometheusPerUnit(const std::string &per_unit_abbreviation)
  {
    return PrometheusExporterUtils::GetPrometheusPerUnit(per_unit_abbreviation);
  }
  static std::string removeUnitPortionInBraces(const std::string &unit)
  {
    return PrometheusExporterUtils::RemoveUnitPortionInBraces(unit);
  }
  static std::string convertRateExpressedToPrometheusUnit(const std::string &rate_expressed_unit)
  {
    return PrometheusExporterUtils::ConvertRateExpressedToPrometheusUnit(rate_expressed_unit);
  }
  static std::string cleanUpString(const std::string &str)
  {
    return PrometheusExporterUtils::CleanUpString(str);
  }
  static std::string getEquivalentPrometheusUnit(const std::string &raw_metric_unit_name)
  {
    return PrometheusExporterUtils::GetEquivalentPrometheusUnit(raw_metric_unit_name);
  }
  static std::string mapToPrometheusName(const std::string &name,
                                         const std::string &unit,
                                         prometheus_client::MetricType prometheus_type,
                                         bool without_units       = false,
                                         bool without_type_suffix = false)
  {
    return PrometheusExporterUtils::MapToPrometheusName(name, unit, prometheus_type, without_units,
                                                        without_type_suffix);
  }
};
}  // namespace metrics
}  // namespace exporter

TEST(PrometheusExporterUtils, TranslateToPrometheusEmptyInputReturnsEmptyCollection)
{
  metric_sdk::ResourceMetrics metrics_data = {};
  auto translated = PrometheusExporterUtils::TranslateToPrometheus(metrics_data);
  ASSERT_EQ(translated.size(), 0U);
  EXPECT_EQ(SerializeMetrics(translated), "");
}

TEST(PrometheusExporterUtils, TranslateToPrometheusIntegerCounter)
{
  TestDataPoints dp;
  metric_sdk::ResourceMetrics metrics_data = dp.CreateSumPointData();

  auto translated = PrometheusExporterUtils::TranslateToPrometheus(metrics_data);
  EXPECT_EQ(SerializeMetrics(translated), ReadTestDataFile("counter.txt"));
}

TEST(PrometheusExporterUtils, TranslateToPrometheusIntegerLastValue)
{
  Resource resource{ResourceAttributes{{"service.name", "test_service"},
                                       {"service.instance.id", "localhost:8000"},
                                       {"custom_resource_attr", "custom_resource_value"}}};
  TestDataPoints dp;
  metric_sdk::ResourceMetrics metrics_data = dp.CreateLastValuePointData();
  metrics_data.resource_                   = &resource;

  auto translated = PrometheusExporterUtils::TranslateToPrometheus(metrics_data);
  EXPECT_EQ(SerializeMetrics(translated), ReadTestDataFile("gauge.txt"));
}

TEST(PrometheusExporterUtils, TranslateToPrometheusHistogramNormal)
{
  Resource resource{ResourceAttributes{{"service.name", "unknown_service"},
                                       {"service.instance.id", "localhost:8001"},
                                       {"custom_resource_attr", "custom_resource_value"}}};
  TestDataPoints dp;
  metric_sdk::ResourceMetrics metrics_data = dp.CreateHistogramPointData();
  metrics_data.resource_                   = &resource;

  auto translated = PrometheusExporterUtils::TranslateToPrometheus(metrics_data);
  EXPECT_EQ(SerializeMetrics(translated), ReadTestDataFile("histogram.txt"));
}

TEST(PrometheusExporterUtils, TranslateToPrometheusUpDownCounter)
{
  TestDataPoints dp;
  metric_sdk::ResourceMetrics metrics_data = dp.CreateUpDownCounterPointData();

  auto translated = PrometheusExporterUtils::TranslateToPrometheus(metrics_data);
  EXPECT_EQ(SerializeMetrics(translated), ReadTestDataFile("up_down_counter.txt"));
}

TEST(PrometheusExporterUtils, TranslateToPrometheusWithoutTargetInfo)
{
  TestDataPoints dp;
  metric_sdk::ResourceMetrics metrics_data = dp.CreateSumPointData();

  auto translated = PrometheusExporterUtils::TranslateToPrometheus(metrics_data, false);
  EXPECT_EQ(SerializeMetrics(translated), ReadTestDataFile("without_target_info.txt"));
}

TEST(PrometheusExporterUtils, TranslateToPrometheusWithoutScopeInfo)
{
  TestDataPoints dp;
  metric_sdk::ResourceMetrics metrics_data = dp.CreateSumPointData();

  auto translated =
      PrometheusExporterUtils::TranslateToPrometheus(metrics_data, true, true, false, false);
  EXPECT_EQ(SerializeMetrics(translated), ReadTestDataFile("without_scope_info.txt"));
}

TEST(PrometheusExporterUtils, TranslateToPrometheusWithoutUnitsAndTypeSuffix)
{
  TestDataPoints dp;
  metric_sdk::ResourceMetrics metrics_data = dp.CreateSumPointData();

  auto translated =
      PrometheusExporterUtils::TranslateToPrometheus(metrics_data, true, false, true, true);
  EXPECT_EQ(SerializeMetrics(translated), ReadTestDataFile("without_units_and_suffix.txt"));
}

namespace
{
class SanitizeTest : public ::testing::Test
{
  Resource resource_{ResourceAttributes{}};
  nostd::unique_ptr<InstrumentationScope> instrumentation_scope_ =
      InstrumentationScope::Create("library_name", "1.2.0");

protected:
  void CheckSanitizeLabel(const std::string &original, const std::string &sanitized)
  {
    metric_sdk::InstrumentDescriptor instrument_descriptor{
        "name", "description", "unit", metric_sdk::InstrumentType::kCounter,
        metric_sdk::InstrumentValueType::kDouble};
    auto result = PrometheusExporterUtils::TranslateToPrometheus(
        {&resource_,
         std::vector<metric_sdk::ScopeMetrics>{
             {instrumentation_scope_.get(),
              std::vector<metric_sdk::MetricData>{
                  {instrument_descriptor, {}, {}, {}, {{{{original, "value"}}, {}}}}}}}},
        false);
    const std::string expected =
        "# HELP name_unit_total description\n"
        "# TYPE name_unit_total counter\n"
        "name_unit_total{" +
        sanitized + "=\"value\",otel_scope_name=\"library_name\",otel_scope_version=\"1.2.0\"} 0\n";
    EXPECT_EQ(SerializeMetrics(result), expected);
  }
};
}  // namespace

TEST_F(SanitizeTest, Label)
{
  CheckSanitizeLabel("name", "name");
  CheckSanitizeLabel("name?", "name_");
  CheckSanitizeLabel("name???", "name_");
  CheckSanitizeLabel("name?__", "name_");
  CheckSanitizeLabel("name?__name", "name_name");
  CheckSanitizeLabel("name?__name:", "name_name_");
}

namespace
{
class AttributeCollisionTest : public ::testing::Test
{
  Resource resource_{ResourceAttributes{}};
  nostd::unique_ptr<InstrumentationScope> instrumentation_scope_ =
      InstrumentationScope::Create("library_name", "1.2.0");
  metric_sdk::InstrumentDescriptor instrument_descriptor_{"library_name", "description", "unit",
                                                          metric_sdk::InstrumentType::kCounter,
                                                          metric_sdk::InstrumentValueType::kDouble};

protected:
  void CheckTranslation(const metric_sdk::PointAttributes &attrs,
                        const std::string &expected_metric_line)
  {
    std::vector<prometheus::MetricFamily> result = PrometheusExporterUtils::TranslateToPrometheus(
        {&resource_,
         std::vector<metric_sdk::ScopeMetrics>{
             {instrumentation_scope_.get(),
              std::vector<metric_sdk::MetricData>{
                  {instrument_descriptor_, {}, {}, {}, {{attrs, {}}}}}}}},
        false);
    const std::string expected =
        "# HELP library_name_unit_total description\n"
        "# TYPE library_name_unit_total counter\n" +
        expected_metric_line + "\n";
    EXPECT_EQ(SerializeMetrics(result), expected);
  }
};
}  // namespace

TEST_F(AttributeCollisionTest, SeparatesDistinctKeys)
{
  CheckTranslation(
      {{"foo.a", "value1"}, {"foo.b", "value2"}},
      R"(library_name_unit_total{foo_a="value1",foo_b="value2",otel_scope_name="library_name",otel_scope_version="1.2.0"} 0)");
}

TEST(PrometheusExporterUtils, PrometheusUnit)
{
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::getPrometheusUnit("d"), "days");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::getPrometheusUnit("h"), "hours");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::getPrometheusUnit("min"), "minutes");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::getPrometheusUnit("s"), "seconds");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::getPrometheusUnit("ms"), "milliseconds");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::getPrometheusUnit("us"), "microseconds");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::getPrometheusUnit("ns"), "nanoseconds");

  ASSERT_EQ(exporter::metrics::SanitizeNameTester::getPrometheusUnit("By"), "bytes");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::getPrometheusUnit("KiBy"), "kibibytes");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::getPrometheusUnit("MiBy"), "mebibytes");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::getPrometheusUnit("GiBy"), "gibibytes");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::getPrometheusUnit("TiBy"), "tibibytes");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::getPrometheusUnit("KBy"), "kilobytes");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::getPrometheusUnit("MBy"), "megabytes");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::getPrometheusUnit("GBy"), "gigabytes");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::getPrometheusUnit("TBy"), "terabytes");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::getPrometheusUnit("By"), "bytes");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::getPrometheusUnit("KBy"), "kilobytes");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::getPrometheusUnit("MBy"), "megabytes");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::getPrometheusUnit("GBy"), "gigabytes");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::getPrometheusUnit("TBy"), "terabytes");

  ASSERT_EQ(exporter::metrics::SanitizeNameTester::getPrometheusUnit("m"), "meters");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::getPrometheusUnit("V"), "volts");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::getPrometheusUnit("A"), "amperes");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::getPrometheusUnit("J"), "joules");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::getPrometheusUnit("W"), "watts");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::getPrometheusUnit("g"), "grams");

  ASSERT_EQ(exporter::metrics::SanitizeNameTester::getPrometheusUnit("Cel"), "celsius");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::getPrometheusUnit("Hz"), "hertz");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::getPrometheusUnit("1"), "");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::getPrometheusUnit("%"), "percent");
}

TEST(PrometheusExporterUtils, PrometheusPerUnit)
{
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::getPrometheusPerUnit("s"), "second");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::getPrometheusPerUnit("m"), "minute");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::getPrometheusPerUnit("h"), "hour");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::getPrometheusPerUnit("d"), "day");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::getPrometheusPerUnit("w"), "week");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::getPrometheusPerUnit("mo"), "month");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::getPrometheusPerUnit("y"), "year");
}

TEST(PrometheusExporterUtils, RemoveUnitPortionInBraces)
{
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::removeUnitPortionInBraces("{unit}"), "");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::removeUnitPortionInBraces("unit{unit}"), "unit");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::removeUnitPortionInBraces("unit_{unit}"),
            "unit_");
}

TEST(PrometheusExporterUtils, ConvertRateExpressedToPrometheusUnit)
{
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::convertRateExpressedToPrometheusUnit("unit/d"),
            "unit_per_day");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::convertRateExpressedToPrometheusUnit("unit/s"),
            "unit_per_second");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::convertRateExpressedToPrometheusUnit("unit/"),
            "unit/");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::convertRateExpressedToPrometheusUnit("unit"),
            "unit");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::convertRateExpressedToPrometheusUnit("unit/m"),
            "unit_per_minute");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::convertRateExpressedToPrometheusUnit("/m"),
            "_per_minute");
}

TEST(PromentheusExporterUtils, PrometheusNameMapping)
{
  // General test cases on unit expansions and name sanitization
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::mapToPrometheusName(
                "sample_metric___name", "g", prometheus::MetricType::Counter),
            "sample_metric_name_grams_total");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::mapToPrometheusName(
                "sample_metric_name", "s", prometheus::MetricType::Counter),
            "sample_metric_name_seconds_total");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::mapToPrometheusName(
                "sample_metric_name", "s", prometheus::MetricType::Gauge),
            "sample_metric_name_seconds");
  // Test without_units & without_type_suffix with Counters and unit = 1
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::mapToPrometheusName(
                "sample_metric_name", "1", prometheus::MetricType::Counter),
            "sample_metric_name_total");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::mapToPrometheusName(
                "sample_metric_name", "1", prometheus::MetricType::Counter, true, false),
            "sample_metric_name_total");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::mapToPrometheusName(
                "sample_metric_name", "1", prometheus::MetricType::Counter, false, true),
            "sample_metric_name");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::mapToPrometheusName(
                "sample_metric_name", "1", prometheus::MetricType::Counter, true, true),
            "sample_metric_name");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::mapToPrometheusName(
                "sample_metric_name", "1", prometheus::MetricType::Counter, true, true),
            "sample_metric_name");
  // Test without_units & without_type_suffix with Counters and non-special units
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::mapToPrometheusName(
                "sample_metric_name", "%", prometheus::MetricType::Counter),
            "sample_metric_name_percent_total");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::mapToPrometheusName(
                "sample_metric_name", "m", prometheus::MetricType::Counter, true, false),
            "sample_metric_name_total");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::mapToPrometheusName(
                "sample_metric_name", "By", prometheus::MetricType::Counter, false, true),
            "sample_metric_name_bytes");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::mapToPrometheusName(
                "sample_metric_name", "s", prometheus::MetricType::Counter, true, true),
            "sample_metric_name");
  // Special case Gauges & ratio
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::mapToPrometheusName(
                "sample_metric_name", "1", prometheus::MetricType::Gauge),
            "sample_metric_name_ratio");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::mapToPrometheusName(
                "sample_metric_name", "1", prometheus::MetricType::Gauge, false, true),
            "sample_metric_name_ratio");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::mapToPrometheusName(
                "sample_metric_name", "1", prometheus::MetricType::Gauge, true, false),
            "sample_metric_name");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::mapToPrometheusName(
                "sample_metric_name", "1", prometheus::MetricType::Gauge, true, true),
            "sample_metric_name");
  // Test without_type_suffix affects only counters
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::mapToPrometheusName(
                "sample_metric_name", "Hz", prometheus::MetricType::Counter),
            "sample_metric_name_hertz_total");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::mapToPrometheusName(
                "sample_metric_name", "Hz", prometheus::MetricType::Counter, false, true),
            "sample_metric_name_hertz");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::mapToPrometheusName(
                "sample_metric_name", "Hz", prometheus::MetricType::Gauge),
            "sample_metric_name_hertz");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::mapToPrometheusName(
                "sample_metric_name", "Hz", prometheus::MetricType::Gauge, false, true),
            "sample_metric_name_hertz");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::mapToPrometheusName(
                "sample_metric_name", "Hz", prometheus::MetricType::Histogram),
            "sample_metric_name_hertz");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::mapToPrometheusName(
                "sample_metric_name", "Hz", prometheus::MetricType::Histogram, false, true),
            "sample_metric_name_hertz");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::mapToPrometheusName(
                "sample_metric_name", "Hz", prometheus::MetricType::Summary),
            "sample_metric_name_hertz");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::mapToPrometheusName(
                "sample_metric_name", "Hz", prometheus::MetricType::Summary, false, true),
            "sample_metric_name_hertz");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::mapToPrometheusName(
                "sample_metric_name", "Hz", prometheus::MetricType::Info),
            "sample_metric_name_hertz");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::mapToPrometheusName(
                "sample_metric_name", "Hz", prometheus::MetricType::Info, false, true),
            "sample_metric_name_hertz");
}

TEST_F(AttributeCollisionTest, JoinsCollidingKeys)
{
  CheckTranslation(
      {{"foo.a", "value1"}, {"foo_a", "value2"}},
      R"(library_name_unit_total{foo_a="value1;value2",otel_scope_name="library_name",otel_scope_version="1.2.0"} 0)");
}

TEST_F(AttributeCollisionTest, DropsInvertedKeys)
{
  CheckTranslation(
      {{"foo.a", "value1"}, {"foo.b", "value2"}, {"foo__a", "value3"}},
      R"(library_name_unit_total{foo_a="value1",foo_b="value2",otel_scope_name="library_name",otel_scope_version="1.2.0"} 0)");
}

TEST(PrometheusExporterUtils, EmptyPointDataAttributesAndNullResourceScopeLabels)
{
  auto scope = InstrumentationScope::Create("library_name", "1.2.0");

  metric_sdk::MetricData empty_metric_data{
      metric_sdk::InstrumentDescriptor{"empty_requests", "description", "1",
                                       metric_sdk::InstrumentType::kCounter,
                                       metric_sdk::InstrumentValueType::kDouble},
      metric_sdk::AggregationTemporality::kCumulative, opentelemetry::common::SystemTimestamp{},
      opentelemetry::common::SystemTimestamp{}, std::vector<metric_sdk::PointDataAttributes>{}};

  metric_sdk::SumPointData sum_point_data{};
  sum_point_data.value_ = 10.0;
  metric_sdk::MetricData metric_data{
      metric_sdk::InstrumentDescriptor{"requests", "description", "1",
                                       metric_sdk::InstrumentType::kCounter,
                                       metric_sdk::InstrumentValueType::kDouble},
      metric_sdk::AggregationTemporality::kCumulative, opentelemetry::common::SystemTimestamp{},
      opentelemetry::common::SystemTimestamp{},
      std::vector<metric_sdk::PointDataAttributes>{{{}, sum_point_data}}};

  metric_sdk::ResourceMetrics data{
      nullptr,
      std::vector<metric_sdk::ScopeMetrics>{
          {scope.get(), std::vector<metric_sdk::MetricData>{empty_metric_data, metric_data}}}};

  auto translated = PrometheusExporterUtils::TranslateToPrometheus(data, false, false);
  EXPECT_EQ(SerializeMetrics(translated), ReadTestDataFile("empty_point_and_null_resource.txt"));
}

TEST(PrometheusExporterUtils, CrossSourceScopeCollisionsAndArrayAttributes)
{
  auto scope = InstrumentationScope::Create("library_name", "1.2.0");

  metric_sdk::PointAttributes point_attrs;
  point_attrs["otel_scope.name"]    = std::string("point_name");
  point_attrs["otel_scope_version"] = std::string("0.9.0");
  point_attrs["tags.a"]             = std::vector<std::string>{"a", "b\"c"};
  point_attrs["tags_a"]             = std::vector<std::string>{"d"};
  point_attrs["counts"]             = std::vector<int>{1, 2, 3};
  point_attrs["doubles"]            = std::vector<double>{1e-9, 2e-9, 1.5};
  point_attrs["flags"]              = std::vector<bool>{true, false};

  metric_sdk::SumPointData sum_point_data{};
  sum_point_data.value_ = 5.0;
  metric_sdk::MetricData metric_data{
      metric_sdk::InstrumentDescriptor{"requests", "description", "1",
                                       metric_sdk::InstrumentType::kCounter,
                                       metric_sdk::InstrumentValueType::kDouble},
      metric_sdk::AggregationTemporality::kCumulative, opentelemetry::common::SystemTimestamp{},
      opentelemetry::common::SystemTimestamp{},
      std::vector<metric_sdk::PointDataAttributes>{{std::move(point_attrs), sum_point_data}}};

  Resource resource{ResourceAttributes{}};
  metric_sdk::ResourceMetrics data{
      &resource, std::vector<metric_sdk::ScopeMetrics>{
                     {scope.get(), std::vector<metric_sdk::MetricData>{metric_data}}}};

  auto translated = PrometheusExporterUtils::TranslateToPrometheus(data, false, false);
  EXPECT_EQ(SerializeMetrics(translated), ReadTestDataFile("scope_collisions_and_arrays.txt"));
}

OPENTELEMETRY_END_NAMESPACE
