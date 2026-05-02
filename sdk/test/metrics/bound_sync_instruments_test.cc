// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/version.h"

#ifdef OPENTELEMETRY_HAVE_METRICS_BOUND_INSTRUMENTS_PREVIEW

#  include <gtest/gtest.h>
#  include <chrono>
#  include <map>
#  include <memory>
#  include <string>
#  include <vector>

#  include "common.h"

#  include "opentelemetry/common/key_value_iterable_view.h"
#  include "opentelemetry/context/context.h"
#  include "opentelemetry/metrics/noop.h"
#  include "opentelemetry/sdk/metrics/aggregation/aggregation_config.h"
#  include "opentelemetry/sdk/metrics/data/metric_data.h"
#  include "opentelemetry/sdk/metrics/data/point_data.h"
#  include "opentelemetry/sdk/metrics/instruments.h"
#  include "opentelemetry/sdk/metrics/state/metric_collector.h"
#  include "opentelemetry/sdk/metrics/state/multi_metric_storage.h"
#  include "opentelemetry/sdk/metrics/state/sync_metric_storage.h"
#  include "opentelemetry/sdk/metrics/view/attributes_processor.h"

#  ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
#    include "opentelemetry/sdk/metrics/exemplar/filter_type.h"
#    include "opentelemetry/sdk/metrics/exemplar/reservoir.h"
#  endif

using namespace opentelemetry::sdk::metrics;
using namespace opentelemetry::common;
using M = std::map<std::string, std::string>;

namespace
{
// Owns AggregationConfig + processor so they outlive the storage.
class StorageHolder
{
public:
  StorageHolder(InstrumentType type       = InstrumentType::kCounter,
                InstrumentValueType vtype = InstrumentValueType::kLong,
                size_t cardinality_limit  = 2000)
      : proc_(new DefaultAttributesProcessor{})
  {
    InstrumentDescriptor desc{"name", "desc", "1unit", type, vtype};
    AggregationType agg_type =
        (type == InstrumentType::kHistogram) ? AggregationType::kHistogram : AggregationType::kSum;
    if (type == InstrumentType::kHistogram)
    {
      hist_cfg_ = std::unique_ptr<HistogramAggregationConfig>(
          new HistogramAggregationConfig(cardinality_limit));
      cfg_ = hist_cfg_.get();
    }
    else
    {
      sum_cfg_ = std::unique_ptr<AggregationConfig>(new AggregationConfig(cardinality_limit));
      cfg_     = sum_cfg_.get();
    }
    storage_ = std::make_shared<SyncMetricStorage>(desc, agg_type, proc_,
#  ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
                                                   ExemplarFilterType::kAlwaysOff,
                                                   ExemplarReservoir::GetNoExemplarReservoir(),
#  endif
                                                   cfg_);
  }

  SyncMetricStorage &operator*() noexcept { return *storage_; }
  SyncMetricStorage *operator->() noexcept { return storage_.get(); }
  std::shared_ptr<SyncMetricStorage> share() const noexcept { return storage_; }

private:
  std::unique_ptr<AggregationConfig> sum_cfg_;
  std::unique_ptr<HistogramAggregationConfig> hist_cfg_;
  const AggregationConfig *cfg_ = nullptr;
  std::shared_ptr<DefaultAttributesProcessor> proc_;
  std::shared_ptr<SyncMetricStorage> storage_;
};

int64_t SumLongFor(SyncMetricStorage &storage,
                   AggregationTemporality temporality,
                   const std::map<std::string, std::string> &filter)
{
  std::shared_ptr<CollectorHandle> collector(new MockCollectorHandle(temporality));
  std::vector<std::shared_ptr<CollectorHandle>> collectors{collector};
  int64_t total = 0;
  storage.Collect(collector.get(), collectors, std::chrono::system_clock::now(),
                  std::chrono::system_clock::now(), [&](const MetricData &md) {
                    for (const auto &p : md.point_data_attr_)
                    {
                      bool match = true;
                      for (const auto &kv : filter)
                      {
                        auto it = p.attributes.find(kv.first);
                        if (it == p.attributes.end() ||
                            opentelemetry::nostd::get<std::string>(it->second) != kv.second)
                        {
                          match = false;
                          break;
                        }
                      }
                      if (match)
                      {
                        const auto &sp = opentelemetry::nostd::get<SumPointData>(p.point_data);
                        total += opentelemetry::nostd::get<int64_t>(sp.value_);
                      }
                    }
                    return true;
                  });
  return total;
}

size_t CollectAndCountPoints(SyncMetricStorage &storage, AggregationTemporality temporality)
{
  std::shared_ptr<CollectorHandle> collector(new MockCollectorHandle(temporality));
  std::vector<std::shared_ptr<CollectorHandle>> collectors{collector};
  size_t count = 0;
  storage.Collect(collector.get(), collectors, std::chrono::system_clock::now(),
                  std::chrono::system_clock::now(), [&](const MetricData &md) {
                    count += md.point_data_attr_.size();
                    return true;
                  });
  return count;
}

bool HasOverflowPoint(SyncMetricStorage &storage, AggregationTemporality temporality)
{
  std::shared_ptr<CollectorHandle> collector(new MockCollectorHandle(temporality));
  std::vector<std::shared_ptr<CollectorHandle>> collectors{collector};
  bool found = false;
  storage.Collect(collector.get(), collectors, std::chrono::system_clock::now(),
                  std::chrono::system_clock::now(), [&](const MetricData &md) {
                    for (const auto &p : md.point_data_attr_)
                    {
                      if (p.attributes.find("otel.metrics.overflow") != p.attributes.end())
                      {
                        found = true;
                      }
                    }
                    return true;
                  });
  return found;
}
}  // namespace

// 1) Bound counter records and exports same datapoint as unbound counter with same attributes.
TEST(BoundSyncInstruments, BoundCounterMatchesUnbound)
{
  StorageHolder holder(InstrumentType::kCounter, InstrumentValueType::kLong);
  M attrs = {{"key", "v"}};
  KeyValueIterableView<M> kv(attrs);

  holder->RecordLong(7, kv, opentelemetry::context::Context{});
  auto bound = holder->Bind(kv);
  ASSERT_NE(bound, nullptr);
  bound->RecordLong(3);

  EXPECT_EQ(SumLongFor(*holder, AggregationTemporality::kDelta, attrs), 10);
  EXPECT_EQ(CollectAndCountPoints(*holder, AggregationTemporality::kDelta), 0u);
}

// 2) Bound histogram records and exports same datapoint as unbound.
TEST(BoundSyncInstruments, BoundHistogramMatchesUnbound)
{
  StorageHolder holder(InstrumentType::kHistogram, InstrumentValueType::kLong);
  M attrs = {{"k", "v"}};
  KeyValueIterableView<M> kv(attrs);

  holder->RecordLong(5, kv, opentelemetry::context::Context{});
  auto bound = holder->Bind(kv);
  ASSERT_NE(bound, nullptr);
  bound->RecordLong(15);

  std::shared_ptr<CollectorHandle> collector(new MockCollectorHandle(AggregationTemporality::kDelta));
  std::vector<std::shared_ptr<CollectorHandle>> collectors{collector};
  bool seen = false;
  holder->Collect(collector.get(), collectors, std::chrono::system_clock::now(),
                  std::chrono::system_clock::now(), [&](const MetricData &md) {
                    for (const auto &p : md.point_data_attr_)
                    {
                      const auto &h = opentelemetry::nostd::get<HistogramPointData>(p.point_data);
                      EXPECT_EQ(h.count_, 2u);
                      EXPECT_EQ(opentelemetry::nostd::get<int64_t>(h.sum_), 20);
                      seen = true;
                    }
                    return true;
                  });
  EXPECT_TRUE(seen);
}

// 3) Bound handles work across delta collection cycles.
TEST(BoundSyncInstruments, BoundSurvivesDeltaCollect)
{
  StorageHolder holder;
  M attrs = {{"k", "v"}};
  KeyValueIterableView<M> kv(attrs);
  auto bound = holder->Bind(kv);

  bound->RecordLong(2);
  EXPECT_EQ(SumLongFor(*holder, AggregationTemporality::kDelta, attrs), 2);

  bound->RecordLong(5);
  bound->RecordLong(7);
  EXPECT_EQ(SumLongFor(*holder, AggregationTemporality::kDelta, attrs), 12);

  EXPECT_EQ(CollectAndCountPoints(*holder, AggregationTemporality::kDelta), 0u);

  bound->RecordLong(1);
  EXPECT_EQ(SumLongFor(*holder, AggregationTemporality::kDelta, attrs), 1);
}

// 4) Bound handles work across cumulative collection cycles.
TEST(BoundSyncInstruments, BoundSurvivesCumulativeCollect)
{
  StorageHolder holder;
  M attrs = {{"k", "v"}};
  KeyValueIterableView<M> kv(attrs);
  auto bound = holder->Bind(kv);

  // Cumulative temporality tracks per-collector accumulated state, so we must
  // reuse the same CollectorHandle across cycles.
  std::shared_ptr<CollectorHandle> collector(
      new MockCollectorHandle(AggregationTemporality::kCumulative));
  std::vector<std::shared_ptr<CollectorHandle>> collectors{collector};

  auto sum_now = [&]() -> int64_t {
    int64_t total = 0;
    holder->Collect(collector.get(), collectors, std::chrono::system_clock::now(),
                    std::chrono::system_clock::now(), [&](const MetricData &md) {
                      for (const auto &p : md.point_data_attr_)
                      {
                        const auto &sp = opentelemetry::nostd::get<SumPointData>(p.point_data);
                        total += opentelemetry::nostd::get<int64_t>(sp.value_);
                      }
                      return true;
                    });
    return total;
  };

  bound->RecordLong(10);
  EXPECT_EQ(sum_now(), 10);

  bound->RecordLong(5);
  EXPECT_EQ(sum_now(), 15);

  bound->RecordLong(20);
  EXPECT_EQ(sum_now(), 35);
}

// 5) Bound + unbound share post-view-filtered attributes -> single merged datapoint.
TEST(BoundSyncInstruments, BoundAndUnboundShareDatapoint)
{
  StorageHolder holder;
  M attrs = {{"k", "v"}};
  KeyValueIterableView<M> kv(attrs);

  auto bound = holder->Bind(kv);
  bound->RecordLong(4);
  holder->RecordLong(6, kv, opentelemetry::context::Context{});
  bound->RecordLong(10);
  holder->RecordLong(2, kv, opentelemetry::context::Context{});

  size_t count = 0;
  std::shared_ptr<CollectorHandle> collector(new MockCollectorHandle(AggregationTemporality::kDelta));
  std::vector<std::shared_ptr<CollectorHandle>> collectors{collector};
  int64_t total = 0;
  holder->Collect(collector.get(), collectors, std::chrono::system_clock::now(),
                  std::chrono::system_clock::now(), [&](const MetricData &md) {
                    count += md.point_data_attr_.size();
                    for (const auto &p : md.point_data_attr_)
                    {
                      const auto &sp = opentelemetry::nostd::get<SumPointData>(p.point_data);
                      total += opentelemetry::nostd::get<int64_t>(sp.value_);
                    }
                    return true;
                  });
  EXPECT_EQ(count, 1u);
  EXPECT_EQ(total, 22);
}

// 6) Binding at cardinality overflow records to the overflow bucket and
//    multiple overflow-bound handles aggregate into the same overflow datapoint.
TEST(BoundSyncInstruments, BindingAtOverflowGoesToOverflowBucket)
{
  // limit = 3: two real distinct keys are admitted; the third new distinct
  // key overflows because the overflow attribute set itself consumes a slot.
  StorageHolder holder(InstrumentType::kCounter, InstrumentValueType::kLong, 3);

  M a1     = {{"k", "1"}};
  auto b1  = holder->Bind(KeyValueIterableView<M>(a1));
  b1->RecordLong(1);

  M a2    = {{"k", "2"}};
  auto b2 = holder->Bind(KeyValueIterableView<M>(a2));
  b2->RecordLong(1);

  M a3    = {{"k", "3"}};
  auto b3 = holder->Bind(KeyValueIterableView<M>(a3));
  b3->RecordLong(100);

  M a4    = {{"k", "4"}};
  auto b4 = holder->Bind(KeyValueIterableView<M>(a4));
  b4->RecordLong(50);

  // Overflow datapoint must equal sum of the overflow-bound writes (100 + 50).
  std::shared_ptr<CollectorHandle> collector(
      new MockCollectorHandle(AggregationTemporality::kDelta));
  std::vector<std::shared_ptr<CollectorHandle>> collectors{collector};
  bool seen          = false;
  int64_t ov_value   = 0;
  holder->Collect(collector.get(), collectors, std::chrono::system_clock::now(),
                  std::chrono::system_clock::now(), [&](const MetricData &md) {
                    for (const auto &p : md.point_data_attr_)
                    {
                      if (p.attributes.find("otel.metrics.overflow") != p.attributes.end())
                      {
                        seen = true;
                        const auto &sp =
                            opentelemetry::nostd::get<SumPointData>(p.point_data);
                        ov_value += opentelemetry::nostd::get<int64_t>(sp.value_);
                      }
                    }
                    return true;
                  });
  EXPECT_TRUE(seen);
  EXPECT_EQ(ov_value, 150);
}

// 7) Noop bound instruments compile and no-op.
TEST(BoundSyncInstruments, NoopBoundCompilesAndNoOps)
{
  opentelemetry::metrics::NoopCounter<uint64_t> counter("name", "", "");
  M attrs    = {{"k", "v"}};
  auto bound = counter.Bind(KeyValueIterableView<M>(attrs));
  ASSERT_NE(bound, nullptr);
  bound->Add(1);
  bound->Add(42);

  opentelemetry::metrics::NoopHistogram<double> hist("name", "", "");
  auto hb = hist.Bind(KeyValueIterableView<M>(attrs));
  ASSERT_NE(hb, nullptr);
  hb->Record(3.14);
}

// Bonus: SyncMultiMetricStorage::Bind fans out to children.
TEST(BoundSyncInstruments, MultiStorageBindFansOut)
{
  StorageHolder h1;
  StorageHolder h2;
  SyncMultiMetricStorage multi;
  multi.AddStorage(h1.share());
  multi.AddStorage(h2.share());

  M attrs = {{"k", "v"}};
  KeyValueIterableView<M> kv(attrs);
  auto bound = multi.Bind(kv);
  ASSERT_NE(bound, nullptr);

  bound->RecordLong(7);
  bound->RecordLong(5);

  EXPECT_EQ(SumLongFor(*h1, AggregationTemporality::kDelta, attrs), 12);
  EXPECT_EQ(SumLongFor(*h2, AggregationTemporality::kDelta, attrs), 12);
}

// Bonus: duplicate Bind() with same attrs returns same entry; combined writes.
TEST(BoundSyncInstruments, DuplicateBindReturnsSameEntry)
{
  StorageHolder holder(InstrumentType::kCounter, InstrumentValueType::kLong, 5);
  M attrs = {{"k", "v"}};
  KeyValueIterableView<M> kv(attrs);
  auto b1 = holder->Bind(kv);
  auto b2 = holder->Bind(kv);
  b1->RecordLong(3);
  b2->RecordLong(4);
  EXPECT_EQ(SumLongFor(*holder, AggregationTemporality::kDelta, attrs), 7);
}

// Bonus: dropped bound entries with no pending data are eventually GC'd at Collect.
TEST(BoundSyncInstruments, DroppedBoundEntriesAreGarbageCollected)
{
  StorageHolder holder(InstrumentType::kCounter, InstrumentValueType::kLong, 5);
  M attrs = {{"k", "v"}};
  KeyValueIterableView<M> kv(attrs);

  // Bind, record once, Collect → dirty rotated to false, entry retained while
  // user holds the shared_ptr.
  {
    auto bound = holder->Bind(kv);
    bound->RecordLong(1);
    EXPECT_EQ(SumLongFor(*holder, AggregationTemporality::kDelta, attrs), 1);
    // bound dropped here; on next Collect, entry will be GC'd because
    // use_count == 1 (only the storage map holds it) and dirty_ is false.
  }

  // Drive a Collect to allow GC.
  EXPECT_EQ(CollectAndCountPoints(*holder, AggregationTemporality::kDelta), 0u);

  // Now binding again: with limit=5 and no live entries, this must succeed
  // without overflow. Fill up to just below the limit using fresh keys.
  std::vector<std::shared_ptr<BoundSyncWritableMetricStorage>> handles;
  for (int i = 0; i < 3; ++i)
  {
    M a    = {{"x", std::to_string(i)}};
    auto h = holder->Bind(KeyValueIterableView<M>(a));
    h->RecordLong(1);
    handles.push_back(h);
  }
  // Expect no overflow yet (3 + 1 < 5).
  EXPECT_FALSE(HasOverflowPoint(*holder, AggregationTemporality::kDelta));
}

// Unified cardinality: an unbound key that already has a datapoint must allow
// Bind() to reuse it without consuming new cardinality.
TEST(BoundSyncInstruments, BindOnExistingUnboundKeyDoesNotOverflow)
{
  // limit = 3: two real keys are admitted; a third new distinct key would
  // overflow (overflow itself consumes a slot).
  StorageHolder holder(InstrumentType::kCounter, InstrumentValueType::kLong, 3);

  M a1 = {{"k", "1"}};
  M a2 = {{"k", "2"}};
  // Two distinct unbound keys consume the two admitted slots (limit=3 with
  // overflow consuming a slot leaves room for two real keys).
  holder->RecordLong(10, KeyValueIterableView<M>(a1), opentelemetry::context::Context{});
  holder->RecordLong(20, KeyValueIterableView<M>(a2), opentelemetry::context::Context{});

  // Bind one of the existing keys: must NOT overflow, must NOT consume a new
  // slot. Bound write goes to the same logical datapoint as the unbound write.
  auto b1 = holder->Bind(KeyValueIterableView<M>(a1));
  ASSERT_NE(b1, nullptr);
  b1->RecordLong(5);

  // Single delta collect: assert no overflow and bound+unbound merge to one point.
  std::shared_ptr<CollectorHandle> collector(
      new MockCollectorHandle(AggregationTemporality::kDelta));
  std::vector<std::shared_ptr<CollectorHandle>> collectors{collector};
  bool overflow_seen = false;
  int64_t a1_sum     = 0;
  holder->Collect(collector.get(), collectors, std::chrono::system_clock::now(),
                  std::chrono::system_clock::now(), [&](const MetricData &md) {
                    for (const auto &p : md.point_data_attr_)
                    {
                      if (p.attributes.find("otel.metrics.overflow") != p.attributes.end())
                        overflow_seen = true;
                      auto it = p.attributes.find("k");
                      if (it != p.attributes.end() &&
                          opentelemetry::nostd::get<std::string>(it->second) == "1")
                      {
                        const auto &sp = opentelemetry::nostd::get<SumPointData>(p.point_data);
                        a1_sum += opentelemetry::nostd::get<int64_t>(sp.value_);
                      }
                    }
                    return true;
                  });
  EXPECT_FALSE(overflow_seen);
  EXPECT_EQ(a1_sum, 15);
}

// Unified cardinality: an existing bound key must allow unbound Record()
// to reuse it without consuming new cardinality, and merge into one datapoint.
TEST(BoundSyncInstruments, UnboundOnExistingBoundKeyDoesNotOverflow)
{
  StorageHolder holder(InstrumentType::kCounter, InstrumentValueType::kLong, 3);

  M a1    = {{"k", "1"}};
  M a2    = {{"k", "2"}};
  auto b1 = holder->Bind(KeyValueIterableView<M>(a1));
  auto b2 = holder->Bind(KeyValueIterableView<M>(a2));
  b1->RecordLong(7);
  b2->RecordLong(3);

  // Unbound write to existing bound key — no overflow, merges with bound.
  holder->RecordLong(13, KeyValueIterableView<M>(a1), opentelemetry::context::Context{});

  // Single delta collect captures all points.
  std::shared_ptr<CollectorHandle> collector(
      new MockCollectorHandle(AggregationTemporality::kDelta));
  std::vector<std::shared_ptr<CollectorHandle>> collectors{collector};
  bool overflow_seen = false;
  int64_t a1_sum = 0, a2_sum = 0;
  holder->Collect(collector.get(), collectors, std::chrono::system_clock::now(),
                  std::chrono::system_clock::now(), [&](const MetricData &md) {
                    for (const auto &p : md.point_data_attr_)
                    {
                      if (p.attributes.find("otel.metrics.overflow") != p.attributes.end())
                        overflow_seen = true;
                      auto it = p.attributes.find("k");
                      if (it == p.attributes.end())
                        continue;
                      const auto &sp = opentelemetry::nostd::get<SumPointData>(p.point_data);
                      auto v         = opentelemetry::nostd::get<int64_t>(sp.value_);
                      auto s         = opentelemetry::nostd::get<std::string>(it->second);
                      if (s == "1")
                        a1_sum += v;
                      else if (s == "2")
                        a2_sum += v;
                    }
                    return true;
                  });
  EXPECT_FALSE(overflow_seen);
  EXPECT_EQ(a1_sum, 20);
  EXPECT_EQ(a2_sum, 3);
}

// Unified cardinality: after delta Collect resets attributes_hashmap_, retained
// bound entries must still count toward the cardinality limit so that new
// unbound keys correctly overflow.
TEST(BoundSyncInstruments, RetainedBoundEntriesCountAfterDeltaCollect)
{
  StorageHolder holder(InstrumentType::kCounter, InstrumentValueType::kLong, 3);

  // Two bound keys consume the two real admitted slots (overflow takes the 3rd).
  M a1    = {{"bound", "1"}};
  M a2    = {{"bound", "2"}};
  auto b1 = holder->Bind(KeyValueIterableView<M>(a1));
  auto b2 = holder->Bind(KeyValueIterableView<M>(a2));
  b1->RecordLong(1);
  b2->RecordLong(1);

  // Delta collect resets attributes_hashmap_ but retains bound_entries_.
  EXPECT_EQ(CollectAndCountPoints(*holder, AggregationTemporality::kDelta), 2u);

  // New unbound key now must overflow because bound entries still count.
  M new_key = {{"unbound", "fresh"}};
  holder->RecordLong(99, KeyValueIterableView<M>(new_key),
                     opentelemetry::context::Context{});

  EXPECT_TRUE(HasOverflowPoint(*holder, AggregationTemporality::kDelta));
}

// Strengthened: assert the overflow datapoint VALUE, not just its presence.
TEST(BoundSyncInstruments, RetainedBoundEntriesOverflowValue)
{
  StorageHolder holder(InstrumentType::kCounter, InstrumentValueType::kLong, 3);
  M a1    = {{"bound", "1"}};
  M a2    = {{"bound", "2"}};
  auto b1 = holder->Bind(KeyValueIterableView<M>(a1));
  auto b2 = holder->Bind(KeyValueIterableView<M>(a2));
  b1->RecordLong(1);
  b2->RecordLong(1);
  EXPECT_EQ(CollectAndCountPoints(*holder, AggregationTemporality::kDelta), 2u);

  M new_key = {{"unbound", "fresh"}};
  holder->RecordLong(99, KeyValueIterableView<M>(new_key), opentelemetry::context::Context{});

  std::shared_ptr<CollectorHandle> collector(
      new MockCollectorHandle(AggregationTemporality::kDelta));
  std::vector<std::shared_ptr<CollectorHandle>> collectors{collector};
  int64_t overflow_sum = 0;
  holder->Collect(collector.get(), collectors, std::chrono::system_clock::now(),
                  std::chrono::system_clock::now(), [&](const MetricData &md) {
                    for (const auto &p : md.point_data_attr_)
                    {
                      if (p.attributes.find("otel.metrics.overflow") == p.attributes.end())
                        continue;
                      const auto &sp = opentelemetry::nostd::get<SumPointData>(p.point_data);
                      overflow_sum += opentelemetry::nostd::get<int64_t>(sp.value_);
                    }
                    return true;
                  });
  EXPECT_EQ(overflow_sum, 99);
}

// No-attribute unbound RecordLong must follow the unified cardinality policy.
// With limit=3, two real distinct bound keys are admitted; the empty-attr
// unbound write would be a third new key and routes to overflow (overflow
// itself consumes a slot).
TEST(BoundSyncInstruments, NoAttributeUnboundFollowsUnifiedPolicyLong)
{
  StorageHolder holder(InstrumentType::kCounter, InstrumentValueType::kLong, 3);
  M a1    = {{"k", "1"}};
  M a2    = {{"k", "2"}};
  auto b1 = holder->Bind(KeyValueIterableView<M>(a1));
  auto b2 = holder->Bind(KeyValueIterableView<M>(a2));
  b1->RecordLong(1);
  b2->RecordLong(1);

  // Empty-attribute unbound write — no slots remain, must overflow.
  holder->RecordLong(42, opentelemetry::context::Context{});

  std::shared_ptr<CollectorHandle> collector(
      new MockCollectorHandle(AggregationTemporality::kDelta));
  std::vector<std::shared_ptr<CollectorHandle>> collectors{collector};
  int64_t overflow_sum = 0;
  holder->Collect(collector.get(), collectors, std::chrono::system_clock::now(),
                  std::chrono::system_clock::now(), [&](const MetricData &md) {
                    for (const auto &p : md.point_data_attr_)
                    {
                      if (p.attributes.find("otel.metrics.overflow") == p.attributes.end())
                        continue;
                      const auto &sp = opentelemetry::nostd::get<SumPointData>(p.point_data);
                      overflow_sum += opentelemetry::nostd::get<int64_t>(sp.value_);
                    }
                    return true;
                  });
  EXPECT_EQ(overflow_sum, 42);
}

// Same coverage for double counter no-attribute path.
TEST(BoundSyncInstruments, NoAttributeUnboundFollowsUnifiedPolicyDouble)
{
  StorageHolder holder(InstrumentType::kCounter, InstrumentValueType::kDouble, 3);
  M a1    = {{"k", "1"}};
  M a2    = {{"k", "2"}};
  auto b1 = holder->Bind(KeyValueIterableView<M>(a1));
  auto b2 = holder->Bind(KeyValueIterableView<M>(a2));
  b1->RecordDouble(1.0);
  b2->RecordDouble(1.0);

  holder->RecordDouble(7.5, opentelemetry::context::Context{});

  std::shared_ptr<CollectorHandle> collector(
      new MockCollectorHandle(AggregationTemporality::kDelta));
  std::vector<std::shared_ptr<CollectorHandle>> collectors{collector};
  double overflow_sum = 0.0;
  holder->Collect(collector.get(), collectors, std::chrono::system_clock::now(),
                  std::chrono::system_clock::now(), [&](const MetricData &md) {
                    for (const auto &p : md.point_data_attr_)
                    {
                      if (p.attributes.find("otel.metrics.overflow") == p.attributes.end())
                        continue;
                      const auto &sp = opentelemetry::nostd::get<SumPointData>(p.point_data);
                      overflow_sum += opentelemetry::nostd::get<double>(sp.value_);
                    }
                    return true;
                  });
  EXPECT_DOUBLE_EQ(overflow_sum, 7.5);
}

// Bound handle outliving its storage must not crash. BoundEntry is self-contained.
TEST(BoundSyncInstruments, BoundOutlivesStorage)
{
  std::shared_ptr<BoundSyncWritableMetricStorage> retained;
  {
    StorageHolder holder;
    M attrs = {{"k", "v"}};
    retained = holder->Bind(KeyValueIterableView<M>(attrs));
    ASSERT_NE(retained, nullptr);
    retained->RecordLong(1);
  }
  // Storage destroyed; bound handle still alive. Recording must not crash.
  retained->RecordLong(2);
  retained.reset();
  SUCCEED();
}

// Mixed bound + unbound across multiple cumulative collections.
TEST(BoundSyncInstruments, MixedBoundUnboundAcrossCumulativeCollections)
{
  StorageHolder holder;
  M attrs = {{"k", "v"}};
  KeyValueIterableView<M> kv(attrs);
  auto bound = holder->Bind(kv);

  std::shared_ptr<CollectorHandle> collector(
      new MockCollectorHandle(AggregationTemporality::kCumulative));
  std::vector<std::shared_ptr<CollectorHandle>> collectors{collector};
  auto sum_now = [&]() -> int64_t {
    int64_t total = 0;
    holder->Collect(collector.get(), collectors, std::chrono::system_clock::now(),
                    std::chrono::system_clock::now(), [&](const MetricData &md) {
                      for (const auto &p : md.point_data_attr_)
                      {
                        const auto &sp = opentelemetry::nostd::get<SumPointData>(p.point_data);
                        total += opentelemetry::nostd::get<int64_t>(sp.value_);
                      }
                      return true;
                    });
    return total;
  };

  bound->RecordLong(3);
  holder->RecordLong(7, kv, opentelemetry::context::Context{});
  EXPECT_EQ(sum_now(), 10);

  holder->RecordLong(5, kv, opentelemetry::context::Context{});
  bound->RecordLong(11);
  EXPECT_EQ(sum_now(), 26);

  bound->RecordLong(4);
  EXPECT_EQ(sum_now(), 30);
}

#endif  // OPENTELEMETRY_HAVE_METRICS_BOUND_INSTRUMENTS_PREVIEW
