#include "opentelemetry/global/provider.h"

#include <gtest/gtest.h>

using opentelemetry::Provider;
using opentelemetry::trace::Tracer;

class TestProvider : public opentelemetry::trace::TracerProvider
{
  Tracer *const GetTracer(string_view library_name, string_view library_version) override
  {
    return nullptr;
  }
};

TEST(Provider, GetTracerProviderDefault)
{
  auto tf = Provider::GetTracerProvider();
  ASSERT_NE(tf, nullptr);
}

TEST(Provider, SetTracerProvider)
{
  auto tf = new TestProvider();
  Provider::SetTracerProvider(tf);
  ASSERT_EQ(Provider::GetTracerProvider(), tf);
}
