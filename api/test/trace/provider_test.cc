// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>

#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/trace/provider.h"
#include "opentelemetry/trace/tracer_provider.h"

using opentelemetry::trace::Provider;
using opentelemetry::trace::Tracer;
using opentelemetry::trace::TracerProvider;

namespace nostd = opentelemetry::nostd;

TEST(Provider, GetTracerProviderDefault)
{
  auto tf = Provider::GetTracerProvider();
  EXPECT_NE(nullptr, tf);
}
