// Copyright (c) 2021 OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/trace/provider.h"
#include "opentelemetry/nostd/shared_ptr.h"

#include <gtest/gtest.h>

using opentelemetry::trace::Provider;
using opentelemetry::trace::Tracer;
using opentelemetry::trace::TracerProvider;

class TestProvider : public TracerProvider
{
  opentelemetry::nostd::shared_ptr<Tracer> GetTracer(
      opentelemetry::nostd::string_view library_name,
      opentelemetry::nostd::string_view library_version) override
  {
    return opentelemetry::nostd::shared_ptr<Tracer>(nullptr);
  }
};

TEST(Provider, GetTracerProviderDefault)
{
  auto tf = Provider::GetTracerProvider();
  EXPECT_NE(nullptr, tf);
}

TEST(Provider, SetTracerProvider)
{
  auto tf = opentelemetry::nostd::shared_ptr<TracerProvider>(new TestProvider());
  Provider::SetTracerProvider(tf);
  ASSERT_EQ(tf, Provider::GetTracerProvider());
}
