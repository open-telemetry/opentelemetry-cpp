// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>

#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/trace/provider.h"
#include "opentelemetry/trace/tracer_provider.h"

#if defined(_MSC_VER)
#  include "opentelemetry/sdk/common/env_variables.h"
using opentelemetry::sdk::common::setenv;
using opentelemetry::sdk::common::unsetenv;
#endif

using opentelemetry::trace::Provider;
using opentelemetry::trace::Tracer;
using opentelemetry::trace::TracerProvider;

namespace nostd = opentelemetry::nostd;

class TestProvider : public TracerProvider
{

#if OPENTELEMETRY_ABI_VERSION_NO >= 2
  nostd::shared_ptr<Tracer> GetTracer(
      nostd::string_view /* name */,
      nostd::string_view /* version */,
      nostd::string_view /* schema_url */,
      const opentelemetry::common::KeyValueIterable * /* attributes */) noexcept override
  {
    return nostd::shared_ptr<Tracer>(nullptr);
  }
#else
  nostd::shared_ptr<Tracer> GetTracer(nostd::string_view /* name */,
                                      nostd::string_view /* version */,
                                      nostd::string_view /* schema_url */) noexcept override
  {
    return nostd::shared_ptr<Tracer>(nullptr);
  }
#endif
};

TEST(Provider, SetTracerProviderDefault)
{
#ifndef NO_GETENV
  unsetenv("OTEL_SDK_DISABLED");
#endif

  auto tf = nostd::shared_ptr<TracerProvider>(new TestProvider());
  Provider::SetTracerProvider(tf);
  ASSERT_EQ(tf, Provider::GetTracerProvider());
}

#ifndef NO_GETENV
TEST(Provider, SetTracerProviderEnabled)
{
  setenv("OTEL_SDK_DISABLED", "false", 1);

  auto tf = nostd::shared_ptr<TracerProvider>(new TestProvider());
  Provider::SetTracerProvider(tf);
  ASSERT_EQ(tf, Provider::GetTracerProvider());

  unsetenv("OTEL_SDK_DISABLED");
}

TEST(Provider, SetTracerProviderDisabled)
{
  setenv("OTEL_SDK_DISABLED", "true", 1);

  auto tf = nostd::shared_ptr<TracerProvider>(new TestProvider());
  Provider::SetTracerProvider(tf);
  ASSERT_NE(tf, Provider::GetTracerProvider());

  unsetenv("OTEL_SDK_DISABLED");
}
#endif
