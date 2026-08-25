// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include "common.h"

#if defined(ENABLE_METRICS_EXEMPLAR_PREVIEW) && !defined(NO_GETENV)
#  include <cstdlib>
#endif

#include "opentelemetry/common/macros.h"
#include "opentelemetry/metrics/meter.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/instrumentationscope/instrumentation_scope.h"
#include "opentelemetry/sdk/instrumentationscope/scope_configurator.h"
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
#  include "opentelemetry/nostd/variant.h"
#  include "opentelemetry/sdk/instrumentationscope/instrumentation_scope.h"
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

TEST(MeterProvider, ConstructorsAreNotNoexcept)
{
  static_assert(!noexcept(MeterProvider()), "MeterProvider construction must be allowed to throw");
  static_assert(!noexcept(MeterProvider(std::unique_ptr<MeterContext>{})),
                "MeterProvider construction must be allowed to throw");
  static_assert(!noexcept(Meter(std::weak_ptr<MeterContext>{})),
                "Meter construction must be allowed to throw");
}

#if OPENTELEMETRY_HAVE_EXCEPTIONS
TEST(MeterProvider, GetMeterReturnsNoopOnConstructionFailure)
{
  auto should_throw = std::make_shared<bool>(true);
  auto throwing_configurator =
      std::make_unique<opentelemetry::sdk::instrumentationscope::ScopeConfigurator<MeterConfig>>(
          opentelemetry::sdk::instrumentationscope::ScopeConfigurator<MeterConfig>::Builder(
              MeterConfig::Default())
              .AddCondition(
                  [should_throw](
                      const opentelemetry::sdk::instrumentationscope::InstrumentationScope &scope) {
                    if (scope.GetName() == "throwing-scope" && *should_throw)
                    {
                      throw std::runtime_error("injected meter construction failure");
                    }
                    return false;
                  },
                  MeterConfig::Default())
              .Build());

  MeterProvider provider(std::unique_ptr<ViewRegistry>(new ViewRegistry()),
                         opentelemetry::sdk::resource::Resource::Create({}),
                         std::move(throwing_configurator));

  auto cached = provider.GetMeter("cached-scope");
  ASSERT_NE(cached, nullptr);
#  ifdef OPENTELEMETRY_RTTI_ENABLED
  ASSERT_NE(dynamic_cast<Meter *>(cached.get()), nullptr);
#  endif
  EXPECT_EQ(provider.GetMeter("cached-scope"), cached);

  auto failed = provider.GetMeter("throwing-scope");
  ASSERT_NE(failed, nullptr);
#  ifdef OPENTELEMETRY_RTTI_ENABLED
  EXPECT_EQ(dynamic_cast<Meter *>(failed.get()), nullptr);
#  endif
  auto failed_counter = failed->CreateUInt64Counter("requests");
  ASSERT_NE(failed_counter, nullptr);
  failed_counter->Add(1);

  auto failed_again = provider.GetMeter("throwing-scope");
  EXPECT_EQ(failed, failed_again);

  *should_throw  = false;
  auto recovered = provider.GetMeter("throwing-scope");
  ASSERT_NE(recovered, nullptr);
  EXPECT_NE(recovered, failed);
#  ifdef OPENTELEMETRY_RTTI_ENABLED
  EXPECT_NE(dynamic_cast<Meter *>(recovered.get()), nullptr);
#  endif
  auto recovered_counter = recovered->CreateUInt64Counter("requests");
  ASSERT_NE(recovered_counter, nullptr);
  recovered_counter->Add(1);

  EXPECT_EQ(provider.GetMeter("cached-scope"), cached);
}
#endif  // OPENTELEMETRY_HAVE_EXCEPTIONS
