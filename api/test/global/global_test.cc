#include "opentelemetry/global/factory.h"

#include <gtest/gtest.h>

using opentelemetry::global::Factory;
using opentelemetry::trace::Tracer;

class TestFactory : public opentelemetry::trace::TracerFactory
{
  Tracer *const getTracer(const string_view &name) { return getTracer(name, ""); }

  Tracer *const getTracer(const string_view &name, const string_view &version) { return nullptr; }
};

TEST(Factory, getTracerFactoryDefault)
{
  auto tf = Factory::getTracerFactory();
  ASSERT_NE(tf, nullptr);
}

TEST(Factory, setTracerFactory)
{
  auto tf = new TestFactory();
  Factory::setTracerFactory(tf);
  ASSERT_EQ(Factory::getTracerFactory(), tf);
}
