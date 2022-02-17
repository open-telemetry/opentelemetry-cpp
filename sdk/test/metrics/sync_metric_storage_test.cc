// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/state/sync_metric_storage.h"
#  include <gtest/gtest.h>
#  include <map>
#  include "opentelemetry/common/key_value_iterable_view.h"
#  include "opentelemetry/sdk/metrics/instruments.h"

using namespace opentelemetry::sdk::metrics;
using M = std::map<std::string, std::string>;

TEST(WritableMetricStorageTest, BasicTests)
{
  InstrumentDescriptor instr_desc = {"name", "desc", "1unit", InstrumentType::kCounter,
                                     InstrumentValueType::kLong};

  opentelemetry::sdk::metrics::SyncMetricStorage storage(instr_desc, AggregationType::kSum);
  EXPECT_NO_THROW(storage.RecordLong(10l));
  EXPECT_NO_THROW(storage.RecordDouble(10.10));
  EXPECT_NO_THROW(storage.RecordLong(
      10l, opentelemetry::common::KeyValueIterableView<M>({{"abc", "123"}, {"xyz", "456"}})));

  EXPECT_NO_THROW(storage.RecordDouble(10.10, opentelemetry::common::KeyValueIterableView<M>({})));
}
#endif
