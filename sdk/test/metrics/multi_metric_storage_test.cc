// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/state/multi_metric_storage.h"
#  include "opentelemetry/common/key_value_iterable_view.h"
#  include "opentelemetry/sdk/metrics/exemplar/no_exemplar_reservoir.h"
#  include "opentelemetry/sdk/metrics/instruments.h"

#  include <gtest/gtest.h>

using namespace opentelemetry;
using namespace opentelemetry::sdk::instrumentationscope;
using namespace opentelemetry::sdk::metrics;

class TestMetricStorage : public SyncWritableMetricStorage
{
public:
  void RecordLong(long value, const opentelemetry::context::Context &context) noexcept override
  {
    num_calls_long++;
  }

  void RecordLong(long value,
                  const opentelemetry::common::KeyValueIterable &attributes,
                  const opentelemetry::context::Context &context) noexcept override
  {
    num_calls_long++;
  }

  void RecordDouble(double value, const opentelemetry::context::Context &context) noexcept override
  {
    num_calls_double++;
  }

  void RecordDouble(double value,
                    const opentelemetry::common::KeyValueIterable &attributes,
                    const opentelemetry::context::Context &context) noexcept override
  {
    num_calls_double++;
  }

  size_t num_calls_long;
  size_t num_calls_double;
};

TEST(MultiMetricStorageTest, BasicTests)
{
  std::shared_ptr<opentelemetry::sdk::metrics::SyncWritableMetricStorage> storage(
      new TestMetricStorage());
  SyncMultiMetricStorage storages{};
  storages.AddStorage(storage);
  EXPECT_NO_THROW(storages.RecordLong(10l, opentelemetry::context::Context{}));
  EXPECT_NO_THROW(storages.RecordLong(20l, opentelemetry::context::Context{}));

  EXPECT_NO_THROW(storages.RecordDouble(10.0, opentelemetry::context::Context{}));
  EXPECT_NO_THROW(storages.RecordLong(30l, opentelemetry::context::Context{}));

  EXPECT_EQ(static_cast<TestMetricStorage *>(storage.get())->num_calls_long, 3);
  EXPECT_EQ(static_cast<TestMetricStorage *>(storage.get())->num_calls_double, 1);
}
#endif
