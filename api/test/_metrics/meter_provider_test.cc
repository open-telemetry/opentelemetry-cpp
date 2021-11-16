// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_METRICS_PREVIEW
#  include <gtest/gtest.h>
#  include "opentelemetry/_metrics/provider.h"
#  include "opentelemetry/nostd/shared_ptr.h"

using opentelemetry::metrics::Meter;
using opentelemetry::metrics::MeterProvider;
using opentelemetry::metrics::Provider;
namespace nostd = opentelemetry::nostd;

class TestProvider : public MeterProvider
{
  nostd::shared_ptr<Meter> GetMeter(nostd::string_view library_name,
                                    nostd::string_view library_version) override
  {
    return nostd::shared_ptr<Meter>(nullptr);
  }
};

TEST(Provider, GetMeterProviderDefault)
{
  auto tf = Provider::GetMeterProvider();
  EXPECT_NE(nullptr, tf);
}

TEST(Provider, SetMeterProvider)
{
  auto tf = nostd::shared_ptr<MeterProvider>(new TestProvider());
  Provider::SetMeterProvider(tf);
  ASSERT_EQ(tf, Provider::GetMeterProvider());
}

TEST(Provider, MultipleMeterProviders)
{
  auto tf = nostd::shared_ptr<MeterProvider>(new TestProvider());
  Provider::SetMeterProvider(tf);
  auto tf2 = nostd::shared_ptr<MeterProvider>(new TestProvider());
  Provider::SetMeterProvider(tf2);

  ASSERT_NE(Provider::GetMeterProvider(), tf);
}
#endif
