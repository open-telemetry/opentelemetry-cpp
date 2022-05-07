// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/trace/provider.h"
#include "opentelemetry/nostd/shared_ptr.h"

#include <gtest/gtest.h>

using opentelemetry::trace::Provider;
using opentelemetry::trace::Tracer;
using opentelemetry::trace::TracerProvider;

namespace nostd = opentelemetry::nostd;

class TestProvider : public TracerProvider
{
  nostd::shared_ptr<Tracer> GetTracer(nostd::string_view library_name,
                                      nostd::string_view library_version,
                                      nostd::string_view schema_url) noexcept override
  {
    return nostd::shared_ptr<Tracer>(nullptr);
  }
};

TEST(Provider, GetTracerProviderDefault)
{
  auto tf = Provider::GetTracerProvider();
  EXPECT_NE(nullptr, tf);
}

TEST(Provider, SetTracerProvider)
{
  auto tf = nostd::shared_ptr<TracerProvider>(new TestProvider());
  Provider::SetTracerProvider(tf);
  ASSERT_EQ(tf, Provider::GetTracerProvider());
}
