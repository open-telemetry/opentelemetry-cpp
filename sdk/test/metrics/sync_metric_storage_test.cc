// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/state/sync_metric_storage.h"
#  include "opentelemetry/common/key_value_iterable_view.h"

#  include <gtest/gtest.h>
#  include <map>

using M = std::map<std::string, std::string>;
TEST(WritableMetricStorageTest, BasicTests)
{
  opentelemetry::sdk::metrics::SyncMetricStorage storage;
  EXPECT_NO_THROW(storage.RecordLong(10l));
  EXPECT_NO_THROW(storage.RecordDouble(10.10));

  EXPECT_NO_THROW(storage.RecordLong(
      10l, opentelemetry::common::KeyValueIterableView<M>({{"abc", "123"}, {"xyz", "456"}})));
  EXPECT_NO_THROW(storage.RecordDouble(10.10, opentelemetry::common::KeyValueIterableView<M>({})));
}
#endif
