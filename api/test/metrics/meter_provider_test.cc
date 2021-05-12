#include <gtest/gtest.h>

#ifdef ENABLE_METRIC_PREVIEW
#  include "opentelemetry/metrics/provider.h"
#  include "opentelemetry/nostd/shared_ptr.h"

using opentelemetry::metrics::Meter;
using opentelemetry::metrics::MeterProvider;
using opentelemetry::metrics::Provider;

class TestProvider : public MeterProvider
{
  opentelemetry::nostd::shared_ptr<Meter> GetMeter(
      opentelemetry::nostd::string_view library_name,
      opentelemetry::nostd::string_view library_version) override
  {
    return opentelemetry::nostd::shared_ptr<Meter>(nullptr);
  }
};

TEST(Provider, GetMeterProviderDefault)
{
  auto tf = Provider::GetMeterProvider();
  EXPECT_NE(nullptr, tf);
}

TEST(Provider, SetMeterProvider)
{
  auto tf = opentelemetry::nostd::shared_ptr<MeterProvider>(new TestProvider());
  Provider::SetMeterProvider(tf);
  ASSERT_EQ(tf, Provider::GetMeterProvider());
}

TEST(Provider, MultipleMeterProviders)
{
  auto tf = opentelemetry::nostd::shared_ptr<MeterProvider>(new TestProvider());
  Provider::SetMeterProvider(tf);
  auto tf2 = opentelemetry::nostd::shared_ptr<MeterProvider>(new TestProvider());
  Provider::SetMeterProvider(tf2);

  ASSERT_NE(Provider::GetMeterProvider(), tf);
}
#else
TEST(Provider, DummyTest)
{
  // empty
}
#endif
