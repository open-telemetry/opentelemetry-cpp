// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <cstdint>
#include <limits>
#include <map>
#include <memory>
#include <string>
#include <utility>

#include "opentelemetry/common/key_value_iterable_view.h"
#include "opentelemetry/context/context.h"
#include "opentelemetry/nostd/utility.h"
#include "opentelemetry/sdk/metrics/instruments.h"
#include "opentelemetry/sdk/metrics/meter_enabled_state.h"
#include "opentelemetry/sdk/metrics/state/metric_storage.h"
#include "opentelemetry/sdk/metrics/state/multi_metric_storage.h"
#include "opentelemetry/sdk/metrics/sync_instruments.h"

using namespace opentelemetry;
using namespace opentelemetry::sdk::metrics;

using M = std::map<std::string, std::string>;

TEST(SyncInstruments, LongCounter)
{
  InstrumentDescriptor instrument_descriptor = {
      "long_counter", "description", "1", InstrumentType::kCounter, InstrumentValueType::kLong};
  std::unique_ptr<SyncWritableMetricStorage> metric_storage(new SyncMultiMetricStorage());
  LongCounter counter(instrument_descriptor, std::move(metric_storage),
                      std::make_shared<MeterEnabledState>());
  counter.Add(10);
  counter.Add(10, opentelemetry::context::Context{});

  counter.Add(10, opentelemetry::common::KeyValueIterableView<M>({{"abc", "123"}, {"xyz", "456"}}));
  counter.Add(10, opentelemetry::common::KeyValueIterableView<M>({{"abc", "123"}, {"xyz", "456"}}),
              opentelemetry::context::Context{});
  counter.Add(10, opentelemetry::common::KeyValueIterableView<M>({}));
  counter.Add(10, opentelemetry::common::KeyValueIterableView<M>({}),
              opentelemetry::context::Context{});
}

TEST(SyncInstruments, DoubleCounter)
{
  InstrumentDescriptor instrument_descriptor = {
      "double_counter", "description", "1", InstrumentType::kCounter, InstrumentValueType::kDouble};
  std::unique_ptr<SyncWritableMetricStorage> metric_storage(new SyncMultiMetricStorage());
  DoubleCounter counter(instrument_descriptor, std::move(metric_storage),
                        std::make_shared<MeterEnabledState>());
  counter.Add(10.10);
  counter.Add(10.10, opentelemetry::context::Context{});

  counter.Add(10.10,
              opentelemetry::common::KeyValueIterableView<M>({{"abc", "123"}, {"xyz", "456"}}));
  counter.Add(10.10,
              opentelemetry::common::KeyValueIterableView<M>({{"abc", "123"}, {"xyz", "456"}}),
              opentelemetry::context::Context{});
  counter.Add(10.10, opentelemetry::common::KeyValueIterableView<M>({}));
  counter.Add(10.10, opentelemetry::common::KeyValueIterableView<M>({}),
              opentelemetry::context::Context{});
}

TEST(SyncInstruments, LongUpDownCounter)
{
  InstrumentDescriptor instrument_descriptor = {"long_updowncounter", "description", "1",
                                                InstrumentType::kUpDownCounter,
                                                InstrumentValueType::kLong};
  std::unique_ptr<SyncWritableMetricStorage> metric_storage(new SyncMultiMetricStorage());
  LongUpDownCounter counter(instrument_descriptor, std::move(metric_storage),
                            std::make_shared<MeterEnabledState>());
  counter.Add(10);
  counter.Add(10, opentelemetry::context::Context{});

  counter.Add(10, opentelemetry::common::KeyValueIterableView<M>({{"abc", "123"}, {"xyz", "456"}}));
  counter.Add(10, opentelemetry::common::KeyValueIterableView<M>({{"abc", "123"}, {"xyz", "456"}}),
              opentelemetry::context::Context{});
  counter.Add(10, opentelemetry::common::KeyValueIterableView<M>({}));
  counter.Add(10, opentelemetry::common::KeyValueIterableView<M>({}),
              opentelemetry::context::Context{});

  // negative values
  counter.Add(-10);
  counter.Add(-10, opentelemetry::context::Context{});

  counter.Add(-10,
              opentelemetry::common::KeyValueIterableView<M>({{"abc", "123"}, {"xyz", "456"}}));
  counter.Add(-10, opentelemetry::common::KeyValueIterableView<M>({{"abc", "123"}, {"xyz", "456"}}),
              opentelemetry::context::Context{});
  counter.Add(-10, opentelemetry::common::KeyValueIterableView<M>({}));
  counter.Add(-10, opentelemetry::common::KeyValueIterableView<M>({}),
              opentelemetry::context::Context{});
}

TEST(SyncInstruments, DoubleUpDownCounter)
{
  InstrumentDescriptor instrument_descriptor = {"double_updowncounter", "description", "1",
                                                InstrumentType::kUpDownCounter,
                                                InstrumentValueType::kDouble};
  std::unique_ptr<SyncWritableMetricStorage> metric_storage(new SyncMultiMetricStorage());
  DoubleUpDownCounter counter(instrument_descriptor, std::move(metric_storage),
                              std::make_shared<MeterEnabledState>());
  counter.Add(10.10);
  counter.Add(10.10, opentelemetry::context::Context{});

  counter.Add(10.10,
              opentelemetry::common::KeyValueIterableView<M>({{"abc", "123"}, {"xyz", "456"}}),
              opentelemetry::context::Context{});
  counter.Add(10.10,
              opentelemetry::common::KeyValueIterableView<M>({{"abc", "123"}, {"xyz", "456"}}));
  counter.Add(10.10, opentelemetry::common::KeyValueIterableView<M>({}),
              opentelemetry::context::Context{});
  counter.Add(10.10, opentelemetry::common::KeyValueIterableView<M>({}));
}

#if OPENTELEMETRY_ABI_VERSION_NO >= 2
TEST(SyncInstruments, LongGauge)
{
  InstrumentDescriptor instrument_descriptor = {"long_gauge", "description", "1",
                                                InstrumentType::kGauge, InstrumentValueType::kLong};
  std::unique_ptr<SyncWritableMetricStorage> metric_storage(new SyncMultiMetricStorage());
  LongGauge gauge(instrument_descriptor, std::move(metric_storage),
                  std::make_shared<MeterEnabledState>());
  gauge.Record(10);
  gauge.Record(10, opentelemetry::context::Context{});

  gauge.Record(10, opentelemetry::common::KeyValueIterableView<M>({{"abc", "123"}, {"xyz", "456"}}),
               opentelemetry::context::Context{});
  gauge.Record(10,
               opentelemetry::common::KeyValueIterableView<M>({{"abc", "123"}, {"xyz", "456"}}));
  gauge.Record(10, opentelemetry::common::KeyValueIterableView<M>({}),
               opentelemetry::context::Context{});
  gauge.Record(10, opentelemetry::common::KeyValueIterableView<M>({}));
}

TEST(SyncInstruments, DoubleGauge)
{
  InstrumentDescriptor instrument_descriptor = {
      "double_gauge", "description", "1", InstrumentType::kGauge, InstrumentValueType::kDouble};
  std::unique_ptr<SyncWritableMetricStorage> metric_storage(new SyncMultiMetricStorage());
  DoubleGauge gauge(instrument_descriptor, std::move(metric_storage),
                    std::make_shared<MeterEnabledState>());
  gauge.Record(10.10);
  gauge.Record(10.10, opentelemetry::context::Context{});

  gauge.Record(10.10,
               opentelemetry::common::KeyValueIterableView<M>({{"abc", "123"}, {"xyz", "456"}}),
               opentelemetry::context::Context{});
  gauge.Record(10.10,
               opentelemetry::common::KeyValueIterableView<M>({{"abc", "123"}, {"xyz", "456"}}));
  gauge.Record(10.10, opentelemetry::common::KeyValueIterableView<M>({}),
               opentelemetry::context::Context{});
  gauge.Record(10.10, opentelemetry::common::KeyValueIterableView<M>({}));
}
#endif

TEST(SyncInstruments, LongHistogram)
{
  InstrumentDescriptor instrument_descriptor = {
      "long_histogram", "description", "1", InstrumentType::kHistogram, InstrumentValueType::kLong};
  std::unique_ptr<SyncWritableMetricStorage> metric_storage(new SyncMultiMetricStorage());
  LongHistogram histogram(instrument_descriptor, std::move(metric_storage),
                          std::make_shared<MeterEnabledState>());
  histogram.Record(10, opentelemetry::context::Context{});

  histogram.Record(10,
                   opentelemetry::common::KeyValueIterableView<M>({{"abc", "123"}, {"xyz", "456"}}),
                   opentelemetry::context::Context{});
  histogram.Record(10, opentelemetry::common::KeyValueIterableView<M>({}),
                   opentelemetry::context::Context{});

#if OPENTELEMETRY_ABI_VERSION_NO >= 2
  histogram.Record(10ULL);
  histogram.Record(10ULL, opentelemetry::common::KeyValueIterableView<M>({}));
#endif
}

TEST(SyncInstruments, DoubleHistogram)
{
  InstrumentDescriptor instrument_descriptor = {"double_histogram", "description", "1",
                                                InstrumentType::kHistogram,
                                                InstrumentValueType::kDouble};
  std::unique_ptr<SyncWritableMetricStorage> metric_storage(new SyncMultiMetricStorage());
  DoubleHistogram histogram(instrument_descriptor, std::move(metric_storage),
                            std::make_shared<MeterEnabledState>());
  histogram.Record(10.10, opentelemetry::context::Context{});
  histogram.Record(-10.10, opentelemetry::context::Context{});  // This is ignored.
  histogram.Record(std::numeric_limits<double>::quiet_NaN(),
                   opentelemetry::context::Context{});  // This is ignored too
  histogram.Record(std::numeric_limits<double>::infinity(),
                   opentelemetry::context::Context{});  // This is ignored too

  histogram.Record(10.10,
                   opentelemetry::common::KeyValueIterableView<M>({{"abc", "123"}, {"xyz", "456"}}),
                   opentelemetry::context::Context{});
  histogram.Record(10.10, opentelemetry::common::KeyValueIterableView<M>({}),
                   opentelemetry::context::Context{});
}

namespace
{

// Sums every value written to it, so tests can check what an instrument actually recorded.
class RecordingStorage : public SyncWritableMetricStorage
{
public:
  void RecordLong(int64_t value, const opentelemetry::context::Context &) noexcept override
  {
    sum_ += value;
  }
  void RecordLong(int64_t value,
                  const opentelemetry::common::KeyValueIterable &,
                  const opentelemetry::context::Context &) noexcept override
  {
    sum_ += value;
  }
  void RecordDouble(double, const opentelemetry::context::Context &) noexcept override {}
  void RecordDouble(double,
                    const opentelemetry::common::KeyValueIterable &,
                    const opentelemetry::context::Context &) noexcept override
  {}

  int64_t sum() const noexcept { return sum_; }

private:
  int64_t sum_ = 0;
};

// Adds value through every Add overload.
void AddWithAllOverloads(LongCounter &counter, uint64_t value)
{
  M attributes = {{"k", "v"}};
  counter.Add(value);
  counter.Add(value, opentelemetry::context::Context{});
  counter.Add(value, opentelemetry::common::KeyValueIterableView<M>(attributes));
  counter.Add(value, opentelemetry::common::KeyValueIterableView<M>(attributes),
              opentelemetry::context::Context{});
}

}  // namespace

TEST(SyncInstruments, DisabledMeterStateSuppressesRecording)
{
  InstrumentDescriptor instrument_descriptor = {
      "long_counter", "description", "1", InstrumentType::kCounter, InstrumentValueType::kLong};
  auto *storage = new RecordingStorage();
  std::unique_ptr<SyncWritableMetricStorage> metric_storage(storage);
  auto meter_enabled_state = std::make_shared<MeterEnabledState>(false);
  LongCounter counter(instrument_descriptor, std::move(metric_storage), meter_enabled_state);

  EXPECT_FALSE(counter.IsEnabled());
  AddWithAllOverloads(counter, 10);
  EXPECT_EQ(storage->sum(), 0);

  // Enabling the meter enables the instrument without recreating it.
  meter_enabled_state->SetEnabled(true);
  EXPECT_TRUE(counter.IsEnabled());
  AddWithAllOverloads(counter, 10);
  EXPECT_EQ(storage->sum(), 40);

  meter_enabled_state->SetEnabled(false);
  EXPECT_FALSE(counter.IsEnabled());
  AddWithAllOverloads(counter, 10);
  EXPECT_EQ(storage->sum(), 40);
}
