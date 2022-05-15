// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/view/view_registry.h"
#  include "opentelemetry/sdk/instrumentationlibrary/instrumentation_library.h"
#  include "opentelemetry/sdk/metrics/instruments.h"
#  include "opentelemetry/sdk/metrics/view/predicate.h"

#  include <gtest/gtest.h>

using namespace opentelemetry::sdk::metrics;
using namespace opentelemetry::sdk::instrumentationlibrary;

TEST(ViewRegistry, FindViewsEmptyRegistry)
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
  auto status =
      registry.FindViews(default_instrument_descriptor, *default_instrumentation_lib.get(),
                         [&count](const View &view) {
                           count++;
                           EXPECT_EQ(view.GetName(), "otel-default-view");
                           EXPECT_EQ(view.GetDescription(), "");
                           EXPECT_EQ(view.GetAggregationType(), AggregationType::kDefault);
                           return true;
                         });
  EXPECT_EQ(count, 1);
  EXPECT_EQ(status, true);
}

TEST(ViewRegistry, FindNonExistingView)
{
  // Add view
  const std::string view_name               = "test_view";
  const std::string view_description        = "test description";
  const std::string instrumentation_name    = "name1";
  const std::string instrumentation_version = "version1";
  const std::string instrumentation_schema  = "schema1";
  const std::string instrument_name         = "testname";
  const InstrumentType instrument_type      = InstrumentType::kCounter;

  std::unique_ptr<InstrumentSelector> instrument_selector{
      new InstrumentSelector(instrument_type, instrument_name)};
  std::unique_ptr<MeterSelector> meter_selector{
      new MeterSelector(instrumentation_name, instrumentation_version, instrumentation_schema)};
  std::unique_ptr<View> view = std::unique_ptr<View>(new View(view_name, view_description));

  ViewRegistry registry;
  registry.AddView(std::move(instrument_selector), std::move(meter_selector), std::move(view));
  InstrumentDescriptor default_instrument_descriptor = {instrument_name,  // name
                                                        "test_descr",     // description
                                                        "1",              // unit
                                                        instrument_type,  // instrument type
                                                        InstrumentValueType::kLong};

  auto default_instrumentation_lib = InstrumentationLibrary::Create(
      instrumentation_name, instrumentation_version, instrumentation_schema);
  int count = 0;
  auto status =
      registry.FindViews(default_instrument_descriptor, *default_instrumentation_lib.get(),
                         [&count, &view_name, &view_description](const View &view) {
                           count++;
#  if HAVE_WORKING_REGEX
                           EXPECT_EQ(view.GetName(), view_name);
                           EXPECT_EQ(view.GetDescription(), view_description);
#  endif
                           return true;
                         });
  EXPECT_EQ(count, 1);
  EXPECT_EQ(status, true);
}
#endif
