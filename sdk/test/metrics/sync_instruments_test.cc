// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/sync_instruments.h"
#  include "opentelemetry/sdk/instrumentationlibrary/instrumentation_library.h"
#  include "opentelemetry/sdk/metrics/measurement_processor.h"

#  include <gtest/gtest.h>

using namespace opentelemetry;
using namespace opentelemetry::sdk::instrumentationlibrary;
using namespace opentelemetry::sdk::metrics;

std::string library_name     = "opentelemetry-cpp";
std::string library_version  = "0.1.0";
auto instrumentation_library = InstrumentationLibrary::Create(library_name, library_version);
SyncMeasurementProcessor measurement_processor;

using M = std::map<std::string, std::string>;

TEST(SyncInstruments, LongCounter)
{
  LongCounter counter("long_counter", instrumentation_library.get(), &measurement_processor,
                      "description", "1");
  EXPECT_NO_THROW(counter.Add(10l));
  EXPECT_NO_THROW(counter.Add(10l));

  EXPECT_NO_THROW(counter.Add(
      10l, opentelemetry::common::KeyValueIterableView<M>({{"abc", "123"}, {"xyz", "456"}})));
  EXPECT_NO_THROW(counter.Add(10l, opentelemetry::common::KeyValueIterableView<M>({})));
}

TEST(SyncInstruments, DoubleCounter)
{
  DoubleCounter counter("double_counter", instrumentation_library.get(), &measurement_processor,
                        "description", "1");
  EXPECT_NO_THROW(counter.Add(10.10));
  EXPECT_NO_THROW(counter.Add(10.10));

  EXPECT_NO_THROW(counter.Add(
      10.10, opentelemetry::common::KeyValueIterableView<M>({{"abc", "123"}, {"xyz", "456"}})));
  EXPECT_NO_THROW(counter.Add(10.10, opentelemetry::common::KeyValueIterableView<M>({})));
}

TEST(SyncInstruments, LongUpDownCounter)
{
  LongUpDownCounter counter("long_up_down_counter", instrumentation_library.get(),
                            &measurement_processor, "description", "1");
  EXPECT_NO_THROW(counter.Add(10l));
  EXPECT_NO_THROW(counter.Add(10l));

  EXPECT_NO_THROW(counter.Add(
      10l, opentelemetry::common::KeyValueIterableView<M>({{"abc", "123"}, {"xyz", "456"}})));
  EXPECT_NO_THROW(counter.Add(10l, opentelemetry::common::KeyValueIterableView<M>({})));
}

TEST(SyncInstruments, DoubleUpDownCounter)
{
  DoubleUpDownCounter counter("double_up_down_counter", instrumentation_library.get(),
                              &measurement_processor, "description", "1");
  EXPECT_NO_THROW(counter.Add(10.10));
  EXPECT_NO_THROW(counter.Add(10.10));

  EXPECT_NO_THROW(counter.Add(
      10.10, opentelemetry::common::KeyValueIterableView<M>({{"abc", "123"}, {"xyz", "456"}})));
  EXPECT_NO_THROW(counter.Add(10.10, opentelemetry::common::KeyValueIterableView<M>({})));
}

TEST(SyncInstruments, LongHistogram)
{
  LongHistogram counter("long_histogram", instrumentation_library.get(), &measurement_processor,
                        "description", "1");
  EXPECT_NO_THROW(counter.Record(10l));
  EXPECT_NO_THROW(counter.Record(10l));

  EXPECT_NO_THROW(counter.Record(
      10l, opentelemetry::common::KeyValueIterableView<M>({{"abc", "123"}, {"xyz", "456"}})));
  EXPECT_NO_THROW(counter.Record(10l, opentelemetry::common::KeyValueIterableView<M>({})));
}

TEST(SyncInstruments, DoubleHistogram)
{
  DoubleHistogram counter("double_histogram", instrumentation_library.get(), &measurement_processor,
                          "description", "1");
  EXPECT_NO_THROW(counter.Record(10.10));
  EXPECT_NO_THROW(counter.Record(10.10));

  EXPECT_NO_THROW(counter.Record(
      10.10, opentelemetry::common::KeyValueIterableView<M>({{"abc", "123"}, {"xyz", "456"}})));
  EXPECT_NO_THROW(counter.Record(10.10, opentelemetry::common::KeyValueIterableView<M>({})));
}

#endif