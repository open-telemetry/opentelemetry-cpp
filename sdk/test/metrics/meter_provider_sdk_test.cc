// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <stdint.h>
#include <atomic>
#include <future>
#include <initializer_list>
#include <set>
#include <string>
#include <thread>
#include <utility>
#include <vector>
#include "common.h"

#if defined(ENABLE_METRICS_EXEMPLAR_PREVIEW) && !defined(NO_GETENV)
#  include <cstdlib>
#endif

#include "opentelemetry/common/macros.h"
#include "opentelemetry/metrics/meter.h"
#include "opentelemetry/metrics/observer_result.h"
#include "opentelemetry/metrics/sync_instruments.h"
#include "opentelemetry/nostd/function_ref.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/unique_ptr.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/instrumentationscope/instrumentation_scope.h"
#include "opentelemetry/sdk/instrumentationscope/scope_configurator.h"
#include "opentelemetry/sdk/metrics/data/point_data.h"
#include "opentelemetry/sdk/metrics/export/metric_producer.h"
#include "opentelemetry/sdk/metrics/instruments.h"
#include "opentelemetry/sdk/metrics/meter.h"
#include "opentelemetry/sdk/metrics/meter_config.h"
#include "opentelemetry/sdk/metrics/meter_context.h"
#include "opentelemetry/sdk/metrics/meter_provider.h"
#include "opentelemetry/sdk/metrics/meter_provider_factory.h"
#include "opentelemetry/sdk/metrics/metric_reader.h"
#include "opentelemetry/sdk/metrics/push_metric_exporter.h"
#include "opentelemetry/sdk/metrics/view/instrument_selector.h"
#include "opentelemetry/sdk/metrics/view/meter_selector.h"
#include "opentelemetry/sdk/metrics/view/view.h"
#include "opentelemetry/sdk/metrics/view/view_registry.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/test_common/sdk/common/scoped_test_log_handler.h"

#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
#  include "opentelemetry/sdk/metrics/exemplar/filter_type.h"
#  include "opentelemetry/sdk/metrics/meter_context_factory.h"

#  ifndef NO_GETENV
#    include "opentelemetry/sdk/instrumentationscope/scope_configurator.h"
#    include "opentelemetry/sdk/metrics/meter_config.h"
#    include "opentelemetry/sdk/metrics/meter_context.h"
#    include "opentelemetry/sdk/metrics/view/view_registry.h"
#    include "opentelemetry/sdk/resource/resource.h"
#  endif

#  if defined(_MSC_VER) && !defined(NO_GETENV)
#    include "opentelemetry/sdk/common/env_variables.h"
using opentelemetry::sdk::common::setenv;
using opentelemetry::sdk::common::unsetenv;
#  endif
#endif

#if OPENTELEMETRY_ABI_VERSION_NO >= 2
#  include <cstdint>
#  include <initializer_list>
#  include <map>
#  include <unordered_map>

#  include "opentelemetry/common/attribute_value.h"
#  include "opentelemetry/nostd/utility.h"
#endif /* OPENTELEMETRY_ABI_VERSION_NO >= 2 */

using namespace opentelemetry::sdk::metrics;
using namespace opentelemetry::sdk::common::internal_log;
using opentelemetry::test_common::ScopedTestLogHandler;

#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
namespace
{

#  ifndef NO_GETENV
constexpr char kMetricsExemplarFilterEnv[] = "OTEL_METRICS_EXEMPLAR_FILTER";
#  endif

class ExemplarFilterEnvironmentTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
#  ifndef NO_GETENV
    unsetenv(kMetricsExemplarFilterEnv);
#  endif
  }

  void TearDown() override
  {
#  ifndef NO_GETENV
    unsetenv(kMetricsExemplarFilterEnv);
#  endif
  }
};

TEST_F(ExemplarFilterEnvironmentTest, UsesTraceBasedDefault)
{
  auto context = MeterContextFactory::Create();
  EXPECT_EQ(context->GetExemplarFilter(), ExemplarFilterType::kTraceBased);
}

#  ifndef NO_GETENV

TEST_F(ExemplarFilterEnvironmentTest, UsesSupportedValuesCaseInsensitively)
{
  const std::pair<const char *, ExemplarFilterType> test_cases[] = {
      {"always_on", ExemplarFilterType::kAlwaysOn},
      {"ALWAYS_ON", ExemplarFilterType::kAlwaysOn},
      {"always_off", ExemplarFilterType::kAlwaysOff},
      {"Always_Off", ExemplarFilterType::kAlwaysOff},
      {"trace_based", ExemplarFilterType::kTraceBased},
      {"TRACE_BASED", ExemplarFilterType::kTraceBased}};

  for (const auto &test_case : test_cases)
  {
    SCOPED_TRACE(test_case.first);
    setenv(kMetricsExemplarFilterEnv, test_case.first, 1);

    auto context = MeterContextFactory::Create();
    EXPECT_EQ(context->GetExemplarFilter(), test_case.second);
  }
}

TEST_F(ExemplarFilterEnvironmentTest, DirectMeterContextReadsEnvironment)
{
  setenv(kMetricsExemplarFilterEnv, "always_off", 1);

  MeterContext context;
  EXPECT_EQ(context.GetExemplarFilter(), ExemplarFilterType::kAlwaysOff);
}

TEST_F(ExemplarFilterEnvironmentTest, EmptyValueUsesTraceBasedDefault)
{
  setenv(kMetricsExemplarFilterEnv, "", 1);

  auto context = MeterContextFactory::Create();
  EXPECT_EQ(context->GetExemplarFilter(), ExemplarFilterType::kTraceBased);
}

TEST_F(ExemplarFilterEnvironmentTest, ExplicitConfigurationOverridesEnvironment)
{
  ScopedTestLogHandler log_handler{LogLevel::Warning};
  setenv(kMetricsExemplarFilterEnv, "invalid", 1);

  auto views    = std::unique_ptr<ViewRegistry>(new ViewRegistry());
  auto resource = opentelemetry::sdk::resource::Resource::Create({});
  auto meter_configurator =
      std::make_unique<opentelemetry::sdk::instrumentationscope::ScopeConfigurator<MeterConfig>>(
          opentelemetry::sdk::instrumentationscope::ScopeConfigurator<MeterConfig>::Builder(
              MeterConfig::Default())
              .Build());

  auto context = MeterContextFactory::Create(
      std::move(views), resource, std::move(meter_configurator), ExemplarFilterType::kAlwaysOff);
  EXPECT_EQ(context->GetExemplarFilter(), ExemplarFilterType::kAlwaysOff);
  EXPECT_TRUE(log_handler.Drain().empty());
}

TEST_F(ExemplarFilterEnvironmentTest, InvalidValueWarnsAndUsesTraceBasedDefault)
{
  ScopedTestLogHandler log_handler{LogLevel::Warning};
  setenv(kMetricsExemplarFilterEnv, "invalid", 1);

  auto context = MeterContextFactory::Create();
  EXPECT_EQ(context->GetExemplarFilter(), ExemplarFilterType::kTraceBased);

  auto logs = log_handler.Drain();
  ASSERT_EQ(logs.size(), 1);
  EXPECT_EQ(logs[0].level, LogLevel::Warning);
  EXPECT_EQ(logs[0].msg,
            "Environment variable <OTEL_METRICS_EXEMPLAR_FILTER> has an invalid value <invalid>, "
            "ignoring");
}

TEST_F(ExemplarFilterEnvironmentTest, MeterProviderFactoryReadsEnvironment)
{
  ScopedTestLogHandler log_handler{LogLevel::Warning};
  setenv(kMetricsExemplarFilterEnv, "invalid", 1);

  auto provider = MeterProviderFactory::Create();
  ASSERT_NE(provider, nullptr);

  auto logs = log_handler.Drain();
  ASSERT_EQ(logs.size(), 1);
  EXPECT_EQ(logs[0].level, LogLevel::Warning);
  EXPECT_EQ(logs[0].msg,
            "Environment variable <OTEL_METRICS_EXEMPLAR_FILTER> has an invalid value <invalid>, "
            "ignoring");
}

TEST_F(ExemplarFilterEnvironmentTest, DirectMeterProviderReadsEnvironment)
{
  ScopedTestLogHandler log_handler{LogLevel::Warning};
  setenv(kMetricsExemplarFilterEnv, "invalid", 1);

  MeterProvider provider;

  auto logs = log_handler.Drain();
  ASSERT_EQ(logs.size(), 1);
  EXPECT_EQ(logs[0].level, LogLevel::Warning);
  EXPECT_EQ(logs[0].msg,
            "Environment variable <OTEL_METRICS_EXEMPLAR_FILTER> has an invalid value <invalid>, "
            "ignoring");
}

TEST_F(ExemplarFilterEnvironmentTest, ProgrammaticConfigurationOverridesEnvironment)
{
  setenv(kMetricsExemplarFilterEnv, "always_on", 1);

  auto context      = MeterContextFactory::Create();
  auto *context_ptr = context.get();
  MeterProvider provider(std::move(context));
  EXPECT_EQ(context_ptr->GetExemplarFilter(), ExemplarFilterType::kAlwaysOn);

  provider.SetExemplarFilter(ExemplarFilterType::kAlwaysOff);
  EXPECT_EQ(context_ptr->GetExemplarFilter(), ExemplarFilterType::kAlwaysOff);
}

#  endif  // NO_GETENV

}  // namespace
#endif  // ENABLE_METRICS_EXEMPLAR_PREVIEW

TEST(MeterProvider, GetMeter)
{
  MeterProvider mp1;
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

  std::unique_ptr<View> view{new View("test_view")};
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
                         {"d", static_cast<unsigned int>(314159)},
                         {"e", static_cast<int32_t>(-20)},
                         {"f", static_cast<uint32_t>(20)},
                         {"g", static_cast<int64_t>(-20)},
                         {"h", static_cast<uint64_t>(20)},
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
      {"a", "string"},
      {"b", false},
      {"c", 314159},
      {"d", static_cast<unsigned int>(314159)},
      {"e", static_cast<int32_t>(-20)},
      {"f", static_cast<uint32_t>(20)},
      {"g", static_cast<int64_t>(-20)},
      {"h", static_cast<uint64_t>(20)},
      {"i", 3.1},
      {"j", "string"}};

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

TEST(MeterProvider, GetMeterEqualityCheck)
{
  auto provider = MeterProviderFactory::Create();

  // providing the same scope names should return the same Meter
  auto meter_library_1a = provider->GetMeter("library_name");
  auto meter_library_1b = provider->GetMeter("library_name");
  EXPECT_EQ(meter_library_1a, meter_library_1b);

  // providing the same scope name and version should return the same meter
  auto meter_version_1a = provider->GetMeter("library_name", "v1.0");
  auto meter_version_1b = provider->GetMeter("library_name", "v1.0");
  EXPECT_EQ(meter_version_1a, meter_version_1b);

  // providing the same name, version, and schema urls should return the same meter
  auto meter_urla = provider->GetMeter("library_name", "v1.0", "url");
  auto meter_urlb = provider->GetMeter("library_name", "v1.0", "url");
  EXPECT_EQ(meter_urla, meter_urlb);
}

TEST(MeterProvider, GetMeterInequalityCheck)
{
  auto provider = MeterProviderFactory::Create();

  auto meter_library_1 = provider->GetMeter("library_1");
  auto meter_library_2 = provider->GetMeter("library_2");
  auto meter_version_1 = provider->GetMeter("library_1", "v1.0");
  auto meter_version_2 = provider->GetMeter("library_1", "v2.0");
  auto meter_url_1     = provider->GetMeter("library_1", "v1.0", "url_1");
  auto meter_url_2     = provider->GetMeter("library_1", "v1.0", "url_2");

  // different scope names should return distinct meters
  EXPECT_NE(meter_library_1, meter_library_2);

  // different scope versions should return distinct meters
  EXPECT_NE(meter_version_1, meter_library_1);
  EXPECT_NE(meter_version_1, meter_version_2);

  // different scope schema urls should return distinct meters
  EXPECT_NE(meter_url_1, meter_library_1);
  EXPECT_NE(meter_url_1, meter_version_1);
  EXPECT_NE(meter_url_1, meter_url_2);
}

#if OPENTELEMETRY_ABI_VERSION_NO >= 2

TEST(MeterProvider, GetMeterEqualityCheckAbiv2)
{
  auto provider = MeterProviderFactory::Create();

  // providing the same name, version, schema url and attributes should return the same meter
  auto meter_attribute1a = provider->GetMeter("library_name", "v1.0", "url", {{"key", "one"}});
  auto meter_attribute1b = provider->GetMeter("library_name", "v1.0", "url", {{"key", "one"}});
  EXPECT_EQ(meter_attribute1a, meter_attribute1b);
}

TEST(MeterProvider, GetMeterInequalityCheckAbiv2)
{
  auto provider = MeterProviderFactory::Create();

  auto meter_1           = provider->GetMeter("library_name", "v1.0", "url");
  auto meter_attribute_1 = provider->GetMeter("library_name", "v1.0", "url", {{"key", "one"}});
  auto meter_attribute_2 = provider->GetMeter("library_name", "v1.0", "url", {{"key", "two"}});

  // different scope attributes should return distinct meters
  EXPECT_NE(meter_attribute_1, meter_1);
  EXPECT_NE(meter_attribute_1, meter_attribute_2);
}

#endif /* OPENTELEMETRY_ABI_VERSION_NO >= 2 */

TEST(MeterProvider, ExplicitShutdownNotWarnOnDestructionCheck)
{
  ScopedTestLogHandler log_handler{LogLevel::Warning};

  auto provider = MeterProviderFactory::Create();
  // Explicit shutdown
  provider->Shutdown();
  auto logs = log_handler.Drain();
  EXPECT_TRUE(logs.empty());

  // Implicit shutdown via destructor
  provider = nullptr;
  logs     = log_handler.Drain();
  EXPECT_TRUE(logs.empty());
}

namespace
{

namespace scope_sdk = opentelemetry::sdk::instrumentationscope;

// Returns a ScopeConfigurator that enables all meters (default config).
std::unique_ptr<scope_sdk::ScopeConfigurator<MeterConfig>> EnableAll()
{
  return std::make_unique<scope_sdk::ScopeConfigurator<MeterConfig>>(
      scope_sdk::ScopeConfigurator<MeterConfig>::Builder(MeterConfig::Default()).Build());
}

// Returns a ScopeConfigurator that disables all meters.
std::unique_ptr<scope_sdk::ScopeConfigurator<MeterConfig>> DisableAll()
{
  return std::make_unique<scope_sdk::ScopeConfigurator<MeterConfig>>(
      scope_sdk::ScopeConfigurator<MeterConfig>::Builder(MeterConfig::Disabled()).Build());
}

// Returns a ScopeConfigurator where the named scope is disabled and all other scopes are enabled.
std::unique_ptr<scope_sdk::ScopeConfigurator<MeterConfig>> DisableByName(
    opentelemetry::nostd::string_view name)
{
  return std::make_unique<scope_sdk::ScopeConfigurator<MeterConfig>>(
      scope_sdk::ScopeConfigurator<MeterConfig>::Builder(MeterConfig::Default())
          .AddConditionNameEquals(name, MeterConfig::Disabled())
          .Build());
}

// Builds a MeterProvider whose MetricReader pointer is returned via the out parameter.
std::shared_ptr<MeterProvider> MakeProvider(
    MetricReader *&reader_out,
    std::unique_ptr<scope_sdk::ScopeConfigurator<MeterConfig>> configurator = EnableAll())
{
  std::unique_ptr<MetricReader> reader{new MockMetricReader()};
  reader_out    = reader.get();
  auto provider = std::make_shared<MeterProvider>(
      std::unique_ptr<ViewRegistry>(new ViewRegistry()),
      opentelemetry::sdk::resource::Resource::Create({}), std::move(configurator));
  provider->AddMetricReader(std::move(reader));
  return provider;
}

// Returns the set of instrumentation scope names present in a collection.
std::set<std::string> CollectScopeNames(MetricReader *reader)
{
  std::set<std::string> scope_names;
  reader->Collect([&scope_names](ResourceMetrics &metric_data) {
    for (const auto &scope_metrics : metric_data.scope_metric_data_)
    {
      scope_names.insert(scope_metrics.scope_->GetName());
    }
    return true;
  });
  return scope_names;
}

// Collected sum of a uint64 counter, or -1 if absent (distinguishes "nothing" from "sum 0").
int64_t CollectCounterSum(MetricReader *reader,
                          const std::string &scope_name,
                          const std::string &instrument_name)
{
  int64_t sum = -1;
  reader->Collect([&](ResourceMetrics &metric_data) {
    for (const auto &scope_metrics : metric_data.scope_metric_data_)
    {
      if (scope_metrics.scope_->GetName() != scope_name)
      {
        continue;
      }
      for (const auto &md : scope_metrics.metric_data_)
      {
        if (md.instrument_descriptor.name_ != instrument_name)
        {
          continue;
        }
        for (const auto &pd : md.point_data_attr_)
        {
          const auto *sum_point = opentelemetry::nostd::get_if<SumPointData>(&pd.point_data);
          if (sum_point != nullptr)
          {
            const auto *value = opentelemetry::nostd::get_if<int64_t>(&sum_point->value_);
            if (value != nullptr)
            {
              sum = (sum == -1) ? *value : sum + *value;
            }
          }
        }
      }
    }
    return true;
  });
  return sum;
}

}  // namespace

TEST(MeterProvider, UpdateMeterConfiguratorDisableByName)
{
  MetricReader *reader{};
  auto provider = MakeProvider(reader);
  ASSERT_NE(nullptr, reader);

  auto meter_disabled_by_update = provider->GetMeter("scope.disabled");
  auto meter_unaffected         = provider->GetMeter("scope.unaffected");

  // Both meters are initially enabled, so both produce working instruments.
  auto counter_disabled_by_update = meter_disabled_by_update->CreateUInt64Counter("counter.a");
  auto counter_unaffected         = meter_unaffected->CreateUInt64Counter("counter.b");

  counter_disabled_by_update->Add(1);
  counter_unaffected->Add(1);
  EXPECT_EQ(CollectScopeNames(reader),
            (std::set<std::string>{"scope.disabled", "scope.unaffected"}));

  provider->UpdateMeterConfigurator(DisableByName("scope.disabled"));

  // The disabled meter is no longer collected, the other scope is untouched.
  counter_disabled_by_update->Add(1);
  counter_unaffected->Add(1);
  EXPECT_EQ(CollectScopeNames(reader), (std::set<std::string>{"scope.unaffected"}));
}

TEST(MeterProvider, UpdateMeterConfiguratorReEnable)
{
  MetricReader *reader{};
  auto provider = MakeProvider(reader);
  ASSERT_NE(nullptr, reader);

  auto meter   = provider->GetMeter("scope.toggle");
  auto counter = meter->CreateUInt64Counter("counter.toggle");

  provider->UpdateMeterConfigurator(DisableAll());
  counter->Add(1);
  EXPECT_TRUE(CollectScopeNames(reader).empty());

  provider->UpdateMeterConfigurator(EnableAll());
  counter->Add(1);
  EXPECT_EQ(CollectScopeNames(reader), (std::set<std::string>{"scope.toggle"}));

  // The measurement recorded while the meter was disabled must not be retained: only the single
  // Add made after re-enabling counts towards the cumulative sum.
  EXPECT_EQ(1, CollectCounterSum(reader, "scope.toggle", "counter.toggle"));
}

// Library code holds instruments for the process lifetime, so re-enabling a meter must not depend
// on the instrument being recreated.
TEST(MeterProvider, UpdateMeterConfiguratorInstrumentCreatedWhileDisabledRecordsAfterEnable)
{
  MetricReader *reader{};
  auto provider = MakeProvider(reader, DisableAll());
  ASSERT_NE(nullptr, reader);

  auto meter   = provider->GetMeter("scope.revived");
  auto counter = meter->CreateUInt64Counter("counter.revived");

  counter->Add(1);
  EXPECT_TRUE(CollectScopeNames(reader).empty());

  provider->UpdateMeterConfigurator(EnableAll());

  counter->Add(1);
  EXPECT_EQ(1, CollectCounterSum(reader, "scope.revived", "counter.revived"));
}

// The mirror case: recording must stop when the meter is disabled.
TEST(MeterProvider, UpdateMeterConfiguratorInstrumentStopsRecordingAfterDisable)
{
  MetricReader *reader{};
  auto provider = MakeProvider(reader);
  ASSERT_NE(nullptr, reader);

  auto meter   = provider->GetMeter("scope.silenced");
  auto counter = meter->CreateUInt64Counter("counter.silenced");

  counter->Add(1);
  EXPECT_EQ(1, CollectCounterSum(reader, "scope.silenced", "counter.silenced"));

  provider->UpdateMeterConfigurator(DisableAll());
  counter->Add(1);
  EXPECT_TRUE(CollectScopeNames(reader).empty());

  // Re-enabling must not reveal the measurement that was recorded while disabled.
  provider->UpdateMeterConfigurator(EnableAll());
  EXPECT_EQ(1, CollectCounterSum(reader, "scope.silenced", "counter.silenced"));
}

TEST(MeterProvider, UpdateMeterConfiguratorAppliesToAllExistingMeters)
{
  MetricReader *reader{};
  auto provider = MakeProvider(reader);
  ASSERT_NE(nullptr, reader);

  std::vector<opentelemetry::nostd::shared_ptr<opentelemetry::metrics::Meter>> meters;
  std::vector<opentelemetry::nostd::unique_ptr<opentelemetry::metrics::Counter<uint64_t>>> counters;
  for (const auto &name : {"scope.one", "scope.two", "scope.three"})
  {
    meters.push_back(provider->GetMeter(name));
    counters.push_back(meters.back()->CreateUInt64Counter("counter"));
  }

  provider->UpdateMeterConfigurator(DisableAll());
  for (auto &counter : counters)
  {
    counter->Add(1);
  }
  EXPECT_TRUE(CollectScopeNames(reader).empty());

  provider->UpdateMeterConfigurator(EnableAll());
  for (auto &counter : counters)
  {
    counter->Add(1);
  }
  EXPECT_EQ(CollectScopeNames(reader),
            (std::set<std::string>{"scope.one", "scope.two", "scope.three"}));
}

TEST(MeterProvider, UpdateMeterConfiguratorNewMeterUsesUpdatedConfig)
{
  MetricReader *reader{};
  auto provider = MakeProvider(reader);
  ASSERT_NE(nullptr, reader);

  provider->UpdateMeterConfigurator(DisableByName("scope.disabled"));

  // Meters created after the update use the updated configurator. A meter that is disabled at
  // instrument creation time returns no-op instruments.
  auto meter_disabled = provider->GetMeter("scope.disabled");
  auto meter_enabled  = provider->GetMeter("scope.enabled");

  auto counter_disabled = meter_disabled->CreateUInt64Counter("counter.a");
  auto counter_enabled  = meter_enabled->CreateUInt64Counter("counter.b");

  counter_disabled->Add(1);
  counter_enabled->Add(1);

  EXPECT_EQ(CollectScopeNames(reader), (std::set<std::string>{"scope.enabled"}));
}

TEST(MeterProvider, UpdateMeterConfiguratorNullIgnored)
{
  MetricReader *reader{};
  auto provider = MakeProvider(reader, DisableByName("scope.disabled"));
  ASSERT_NE(nullptr, reader);

  auto meter   = provider->GetMeter("scope.enabled");
  auto counter = meter->CreateUInt64Counter("counter");

  provider->UpdateMeterConfigurator(nullptr);

  // The existing configurator is retained.
  counter->Add(1);
  EXPECT_EQ(CollectScopeNames(reader), (std::set<std::string>{"scope.enabled"}));

  auto meter_disabled   = provider->GetMeter("scope.disabled");
  auto counter_disabled = meter_disabled->CreateUInt64Counter("counter.disabled");
  counter_disabled->Add(1);
  EXPECT_EQ(CollectScopeNames(reader), (std::set<std::string>{"scope.enabled"}));
}

TEST(MeterProvider, UpdateMeterConfiguratorConcurrentGetMeter)
{
  MetricReader *reader{};
  auto provider = MakeProvider(reader);
  ASSERT_NE(nullptr, reader);

  constexpr int kUpdateCount = 200;

  std::atomic<bool> stop{false};
  std::promise<void> worker_ready;
  std::future<void> worker_ready_future = worker_ready.get_future();

  // Worker: create meters and record measurements while the configurator is being replaced.
  std::thread worker([&] {
    worker_ready.set_value();
    int i = 0;
    while (!stop.load(std::memory_order_relaxed))
    {
      auto meter   = provider->GetMeter("scope." + std::to_string(i++ % 8));
      auto counter = meter->CreateUInt64Counter("counter");
      counter->Add(1);
    }
  });

  worker_ready_future.wait();

  for (int i = 0; i < kUpdateCount; ++i)
  {
    provider->UpdateMeterConfigurator(i % 2 == 0 ? DisableAll() : EnableAll());
  }

  stop.store(true, std::memory_order_relaxed);
  worker.join();

  // With the final configurator enabling all scopes, every existing meter must be collectable.
  provider->UpdateMeterConfigurator(EnableAll());
  auto meter   = provider->GetMeter("scope.final");
  auto counter = meter->CreateUInt64Counter("counter.final");
  counter->Add(1);
  EXPECT_FALSE(CollectScopeNames(reader).empty());
}

// Guards the lock order between UpdateMeterConfigurator (lock_) and collection (meter_lock_ then
// lock_, via an observable callback calling GetMeter). Hangs rather than fails on regression.
TEST(MeterProvider, UpdateMeterConfiguratorConcurrentWithCollectDoesNotDeadlock)
{
  MetricReader *reader{};
  auto provider = MakeProvider(reader);
  ASSERT_NE(nullptr, reader);

  auto meter = provider->GetMeter("scope.observable");

  // Re-entering the provider from inside collection is what takes lock_ under meter_lock_.
  static MeterProvider *callback_provider = provider.get();
  auto observable                         = meter->CreateInt64ObservableCounter("obs.counter");
  observable->AddCallback(
      [](opentelemetry::metrics::ObserverResult, void *) noexcept {
        if (callback_provider != nullptr)
        {
          auto reentrant = callback_provider->GetMeter("scope.observable");
          (void)reentrant;
        }
      },
      nullptr);

  constexpr int kUpdateCount = 500;

  std::atomic<bool> stop{false};
  std::promise<void> collector_ready;
  std::future<void> collector_ready_future = collector_ready.get_future();

  std::thread collector([&] {
    collector_ready.set_value();
    while (!stop.load(std::memory_order_relaxed))
    {
      reader->Collect([](ResourceMetrics &) { return true; });
    }
  });

  collector_ready_future.wait();

  for (int i = 0; i < kUpdateCount; ++i)
  {
    provider->UpdateMeterConfigurator(i % 2 == 0 ? DisableAll() : EnableAll());
  }

  stop.store(true, std::memory_order_relaxed);
  collector.join();

  callback_provider = nullptr;

  provider->UpdateMeterConfigurator(EnableAll());
}

TEST(MeterProvider, SetMeterConfiguratorNullIgnoredOnContext)
{
  ScopedTestLogHandler log_handler{LogLevel::Error};

  auto context = std::make_shared<MeterContext>(std::unique_ptr<ViewRegistry>(new ViewRegistry()),
                                                opentelemetry::sdk::resource::Resource::Create({}),
                                                DisableAll());

  context->SetMeterConfigurator(nullptr);

  auto logs = log_handler.Drain();
  ASSERT_EQ(logs.size(), 1);
  EXPECT_NE(logs[0].msg.find("must not be null"), std::string::npos);

  // The configurator passed at construction is retained.
  auto scope = opentelemetry::sdk::instrumentationscope::InstrumentationScope::Create("scope");
  EXPECT_FALSE(context->GetMeterConfigurator().ComputeConfig(*scope).IsEnabled());
}

TEST(MeterProvider, MeterWithExpiredContextIsEnabledByDefault)
{
  ScopedTestLogHandler log_handler{LogLevel::Error};

  std::weak_ptr<MeterContext> expired_context;
  {
    auto context = std::make_shared<MeterContext>(
        std::unique_ptr<ViewRegistry>(new ViewRegistry()),
        opentelemetry::sdk::resource::Resource::Create({}), DisableAll());
    expired_context = context;
  }
  ASSERT_TRUE(expired_context.expired());

  // A Meter cannot compute its config without a context, so it falls back to the default config.
  Meter meter{expired_context,
              opentelemetry::sdk::instrumentationscope::InstrumentationScope::Create("scope")};

  auto logs = log_handler.Drain();
  ASSERT_EQ(logs.size(), 1);
  EXPECT_NE(logs[0].msg.find("The metric context is invalid"), std::string::npos);

  // MeterConfig::Default() is enabled, so instrument creation is not short circuited.
  EXPECT_NE(nullptr, meter.CreateUInt64Counter("counter"));
}
