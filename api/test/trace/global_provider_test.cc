#include "opentelemetry/trace/global_provider.h"

#include <gtest/gtest.h>

using opentelemetry::trace::Provider;
using opentelemetry::trace::Tracer;

class TestProvider : public opentelemetry::trace::TracerProvider
{
  Tracer *const GetTracer(opentelemetry::nostd::string_view library_name,
                          opentelemetry::nostd::string_view library_version,
                          const opentelemetry::trace::TracerOptions &tracer_options) override
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
  // Capture the default provider constructed in order to free it
  auto default_provider = Provider::SetTracerProvider(tf);
  ASSERT_EQ(Provider::GetTracerProvider(), tf);
  auto res = Provider::SetTracerProvider(nullptr);
  ASSERT_EQ(tf, res);
  delete res;
  delete default_provider;
}
