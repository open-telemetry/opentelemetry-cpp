#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/trace/provider.h"

#include <gtest/gtest.h>

using opentelemetry::trace::Provider;
using opentelemetry::trace::TracerProvider;
using opentelemetry::trace::Tracer;

class TestProvider : public TracerProvider
{
    opentelemetry::nostd::shared_ptr<Tracer> GetTracer(opentelemetry::nostd::string_view library_name,
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
  // Capture the default provider constructed in order to replace it
  Provider::SetTracerProvider(tf);
  ASSERT_EQ(tf, Provider::GetTracerProvider());

  Provider::SetTracerProvider(nullptr);
  auto default_provider = Provider::GetTracerProvider();

  Provider::SetTracerProvider(nullptr);
}
