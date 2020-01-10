#include "opentelemetry/global/factory.h"

#include <gtest/gtest.h>

using opentelemetry::global::Factory;
using opentelemetry::trace::Tracer;

class TestFactory : public opentelemetry::trace::TracerFactory
{
  Tracer *const GetTracer(string_view library_name, string_view library_version) override { return nullptr; }
};

TEST(Factory, GetTracerFactoryDefault)
{
  auto tf = Factory::GetTracerFactory();
  ASSERT_NE(tf, nullptr);
}

TEST(Factory, SetTracerFactory)
{
  auto tf = new TestFactory();
  Factory::SetTracerFactory(tf);
  ASSERT_EQ(Factory::GetTracerFactory(), tf);
}
