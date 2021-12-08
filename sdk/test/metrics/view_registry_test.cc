// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/view/view_registry.h"
#  include <gtest/gtest.h>
#  include "opentelemetry/sdk/instrumentationlibrary/instrumentation_library.h"
#  include "opentelemetry/sdk/metrics/instruments.h"

using namespace opentelemetry::sdk::metrics;
using namespace opentelemetry::sdk::instrumentationlibrary;

TEST(ViewRegistry, FindViews)
{
  InstrumentDescriptor default_instrument_descriptor = {
      "test_name",               // name
      "test_descr",              // description
      "1",                       // unit
      InstrumentType::kCounter,  // instrument type
      InstrumentValueType::kLong};

  auto default_instrumentation_lib =
      InstrumentationLibrary::Create("default", "1.0.0", "https://opentelemetry.io/schemas/1.7.0");
  int count = 0;
  ViewRegistry registry;
  auto status = registry.FindViews(default_instrument_descriptor,
                                   *default_instrumentation_lib.get(), [&count](const View &view) {
                                     count++;
                                     EXPECT_EQ(view.GetName(), kDefaultViewName);
                                     EXPECT_EQ(view.GetDescription(), kDefaultViewDescription);
                                     return true;
                                   });
  EXPECT_EQ(count, 1);
  EXPECT_EQ(status, true);
}
#endif