// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/sync_instruments.h"
#  include "opentelemetry/context/context.h"
#  include "opentelemetry/sdk/instrumentationlibrary/instrumentation_library.h"
#  include "opentelemetry/sdk/metrics/exemplar/no_exemplar_reservoir.h"
#  include "opentelemetry/sdk/metrics/state/multi_metric_storage.h"

#  include <gtest/gtest.h>
#  include <cmath>
#  include <limits>

using namespace opentelemetry;
using namespace opentelemetry::sdk::instrumentationlibrary;
using namespace opentelemetry::sdk::metrics;

auto instrumentation_library = InstrumentationLibrary::Create("opentelemetry-cpp", "0.1.0");

using M = std::map<std::string, std::string>;

TEST(SyncInstruments, LongCounter)
{
  InstrumentDescriptor instrument_descriptor = {
      "long_counter", "description", "1", InstrumentType::kCounter, InstrumentValueType::kLong};
  std::unique_ptr<WritableMetricStorage> metric_storage(new MultiMetricStorage());
  LongCounter counter(instrument_descriptor, std::move(metric_storage));
  EXPECT_NO_THROW(counter.Add(10l));
  EXPECT_NO_THROW(counter.Add(10l, opentelemetry::context::Context{}));

  EXPECT_NO_THROW(counter.Add(
      10l, opentelemetry::common::KeyValueIterableView<M>({{"abc", "123"}, {"xyz", "456"}})));
  EXPECT_NO_THROW(counter.Add(
      10l, opentelemetry::common::KeyValueIterableView<M>({{"abc", "123"}, {"xyz", "456"}}),
      opentelemetry::context::Context{}));
  EXPECT_NO_THROW(counter.Add(10l, opentelemetry::common::KeyValueIterableView<M>({})));
  EXPECT_NO_THROW(counter.Add(10l, opentelemetry::common::KeyValueIterableView<M>({}),
                              opentelemetry::context::Context{}));
}

TEST(SyncInstruments, DoubleCounter)
{
  InstrumentDescriptor instrument_descriptor = {
      "double_counter", "description", "1", InstrumentType::kCounter, InstrumentValueType::kDouble};
  std::unique_ptr<WritableMetricStorage> metric_storage(new MultiMetricStorage());
  DoubleCounter counter(instrument_descriptor, std::move(metric_storage));
  EXPECT_NO_THROW(counter.Add(10.10));
  EXPECT_NO_THROW(counter.Add(10.10, opentelemetry::context::Context{}));

  EXPECT_NO_THROW(counter.Add(
      10.10, opentelemetry::common::KeyValueIterableView<M>({{"abc", "123"}, {"xyz", "456"}})));
  EXPECT_NO_THROW(counter.Add(
      10.10, opentelemetry::common::KeyValueIterableView<M>({{"abc", "123"}, {"xyz", "456"}}),
      opentelemetry::context::Context{}));
  EXPECT_NO_THROW(counter.Add(10.10, opentelemetry::common::KeyValueIterableView<M>({})));
  EXPECT_NO_THROW(counter.Add(10.10, opentelemetry::common::KeyValueIterableView<M>({}),
                              opentelemetry::context::Context{}));
}

TEST(SyncInstruments, LongUpDownCounter)
{
  InstrumentDescriptor instrument_descriptor = {"long_updowncounter", "description", "1",
                                                InstrumentType::kUpDownCounter,
                                                InstrumentValueType::kLong};
  std::unique_ptr<WritableMetricStorage> metric_storage(new MultiMetricStorage());
  LongUpDownCounter counter(instrument_descriptor, std::move(metric_storage));
  EXPECT_NO_THROW(counter.Add(10l));
  EXPECT_NO_THROW(counter.Add(10l, opentelemetry::context::Context{}));

  EXPECT_NO_THROW(counter.Add(
      10l, opentelemetry::common::KeyValueIterableView<M>({{"abc", "123"}, {"xyz", "456"}})));
  EXPECT_NO_THROW(counter.Add(
      10l, opentelemetry::common::KeyValueIterableView<M>({{"abc", "123"}, {"xyz", "456"}}),
      opentelemetry::context::Context{}));
  EXPECT_NO_THROW(counter.Add(10l, opentelemetry::common::KeyValueIterableView<M>({})));
  EXPECT_NO_THROW(counter.Add(10l, opentelemetry::common::KeyValueIterableView<M>({}),
                              opentelemetry::context::Context{}));
}

TEST(SyncInstruments, DoubleUpDownCounter)
{
  InstrumentDescriptor instrument_descriptor = {"double_updowncounter", "description", "1",
                                                InstrumentType::kUpDownCounter,
                                                InstrumentValueType::kDouble};
  std::unique_ptr<WritableMetricStorage> metric_storage(new MultiMetricStorage());
  DoubleUpDownCounter counter(instrument_descriptor, std::move(metric_storage));
  EXPECT_NO_THROW(counter.Add(10.10));
  EXPECT_NO_THROW(counter.Add(10.10, opentelemetry::context::Context{}));

  EXPECT_NO_THROW(counter.Add(
      10.10, opentelemetry::common::KeyValueIterableView<M>({{"abc", "123"}, {"xyz", "456"}}),
      opentelemetry::context::Context{}));
  EXPECT_NO_THROW(counter.Add(
      10.10, opentelemetry::common::KeyValueIterableView<M>({{"abc", "123"}, {"xyz", "456"}})));
  EXPECT_NO_THROW(counter.Add(10.10, opentelemetry::common::KeyValueIterableView<M>({}),
                              opentelemetry::context::Context{}));
  EXPECT_NO_THROW(counter.Add(10.10, opentelemetry::common::KeyValueIterableView<M>({})));
}

TEST(SyncInstruments, LongHistogram)
{
  InstrumentDescriptor instrument_descriptor = {
      "long_histogram", "description", "1", InstrumentType::kHistogram, InstrumentValueType::kLong};
  std::unique_ptr<WritableMetricStorage> metric_storage(new MultiMetricStorage());
  LongHistogram counter(instrument_descriptor, std::move(metric_storage));
  EXPECT_NO_THROW(counter.Record(10l, opentelemetry::context::Context{}));
  EXPECT_NO_THROW(counter.Record(-10l, opentelemetry::context::Context{}));  // This is ignored

  EXPECT_NO_THROW(counter.Record(
      10l, opentelemetry::common::KeyValueIterableView<M>({{"abc", "123"}, {"xyz", "456"}}),
      opentelemetry::context::Context{}));
  EXPECT_NO_THROW(counter.Record(10l, opentelemetry::common::KeyValueIterableView<M>({}),
                                 opentelemetry::context::Context{}));
}

TEST(SyncInstruments, DoubleHistogram)
{
  InstrumentDescriptor instrument_descriptor = {"double_histogram", "description", "1",
                                                InstrumentType::kHistogram,
                                                InstrumentValueType::kDouble};
  std::unique_ptr<WritableMetricStorage> metric_storage(new MultiMetricStorage());
  DoubleHistogram counter(instrument_descriptor, std::move(metric_storage));
  EXPECT_NO_THROW(counter.Record(10.10, opentelemetry::context::Context{}));
  EXPECT_NO_THROW(counter.Record(-10.10, opentelemetry::context::Context{}));  // This is ignored.
  EXPECT_NO_THROW(counter.Record(std::numeric_limits<double>::quiet_NaN(),
                                 opentelemetry::context::Context{}));  // This is ignored too
  EXPECT_NO_THROW(counter.Record(std::numeric_limits<double>::infinity(),
                                 opentelemetry::context::Context{}));  // This is ignored too

  EXPECT_NO_THROW(counter.Record(
      10.10, opentelemetry::common::KeyValueIterableView<M>({{"abc", "123"}, {"xyz", "456"}}),
      opentelemetry::context::Context{}));
  EXPECT_NO_THROW(counter.Record(10.10, opentelemetry::common::KeyValueIterableView<M>({}),
                                 opentelemetry::context::Context{}));
}

#endif