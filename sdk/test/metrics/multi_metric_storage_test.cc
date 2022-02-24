// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/state/multi_metric_storage.h"
#  include "opentelemetry/common/key_value_iterable_view.h"
#  include "opentelemetry/sdk/metrics/instruments.h"

#  include <gtest/gtest.h>

using namespace opentelemetry;
using namespace opentelemetry::sdk::instrumentationlibrary;
using namespace opentelemetry::sdk::metrics;

class TestMetricStorage : public WritableMetricStorage
{
public:
  void RecordLong(long value) noexcept override { num_calls_long++; }

  void RecordLong(long value,
                  const opentelemetry::common::KeyValueIterable &attributes) noexcept override
  {
    num_calls_long++;
  }

  void RecordDouble(double value) noexcept override { num_calls_double++; }

  void RecordDouble(double value,
                    const opentelemetry::common::KeyValueIterable &attributes) noexcept override
  {
    num_calls_double++;
  }

  size_t num_calls_long;
  size_t num_calls_double;
};

TEST(MultiMetricStorageTest, BasicTests)
{
  std::shared_ptr<opentelemetry::sdk::metrics::WritableMetricStorage> storage(
      new TestMetricStorage());
  MultiMetricStorage storages;
  storages.AddStorage(storage);
  EXPECT_NO_THROW(storages.RecordLong(10l));
  EXPECT_NO_THROW(storages.RecordLong(20l));

  EXPECT_NO_THROW(storages.RecordDouble(10.0));
  EXPECT_NO_THROW(storages.RecordLong(30l));

  EXPECT_EQ(static_cast<TestMetricStorage *>(storage.get())->num_calls_long, 3);
  EXPECT_EQ(static_cast<TestMetricStorage *>(storage.get())->num_calls_double, 1);
}
#endif
