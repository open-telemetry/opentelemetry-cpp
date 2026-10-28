// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <string>

#include "opentelemetry/metrics/meter_provider.h"
#include "opentelemetry/metrics/noop.h"
#include "opentelemetry/metrics/provider.h"
#include "opentelemetry/nostd/shared_ptr.h"

using opentelemetry::metrics::MeterProvider;
using opentelemetry::metrics::NoopMeterProvider;
using opentelemetry::metrics::Provider;

TEST(Provider, GetMeterProviderDefault)
{
  auto tf = Provider::GetMeterProvider();
  EXPECT_NE(nullptr, tf);
}

TEST(Provider, SetMeterProvider)
{
  auto tf = opentelemetry::nostd::shared_ptr<MeterProvider>(new NoopMeterProvider());
  Provider::SetMeterProvider(tf);
  ASSERT_EQ(tf, Provider::GetMeterProvider());
}

TEST(Provider, MultipleMeterProviders)
{
  auto tf = opentelemetry::nostd::shared_ptr<MeterProvider>(new NoopMeterProvider());
  Provider::SetMeterProvider(tf);
  auto tf2 = opentelemetry::nostd::shared_ptr<MeterProvider>(new NoopMeterProvider());
  Provider::SetMeterProvider(tf2);

  ASSERT_NE(Provider::GetMeterProvider(), tf);
}

TEST(Provider, SetNullMeterProvider)
{
  auto tf = opentelemetry::nostd::shared_ptr<MeterProvider>(new NoopMeterProvider());
  Provider::SetMeterProvider(tf);
  ASSERT_EQ(tf, Provider::GetMeterProvider());

  // Setting a null MeterProvider installs a no-op MeterProvider.
  Provider::SetMeterProvider(opentelemetry::nostd::shared_ptr<MeterProvider>());

  auto noop = Provider::GetMeterProvider();
  ASSERT_NE(nullptr, noop);
  ASSERT_NE(tf, noop);

  // The no-op MeterProvider is usable, it does not crash on use.
  EXPECT_NE(nullptr, noop->GetMeter("test"));
}
