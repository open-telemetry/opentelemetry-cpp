// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include "opentelemetry/metrics_new/noop.h"
#include "opentelemetry/metrics_new/provider.h"
#include "opentelemetry/nostd/shared_ptr.h"

using opentelemetry::metrics_new::Meter;
using opentelemetry::metrics_new::MeterProvider;
using opentelemetry::metrics_new::NoopMeterProvider;
using opentelemetry::metrics_new::Provider;

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
