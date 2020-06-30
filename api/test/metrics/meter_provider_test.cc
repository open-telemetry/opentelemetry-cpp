#include "opentelemetry/metrics/provider.h"
#include "opentelemetry/nostd/shared_ptr.h"

#include <gtest/gtest.h>

using opentelemetry::meter::Provider;
using opentelemetry::meter::Meter;
using opentelemetry::meter::MeterProvider;

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
