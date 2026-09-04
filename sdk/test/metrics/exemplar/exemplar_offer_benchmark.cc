// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <benchmark/benchmark.h>

#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW

#  include <cstddef>
#  include <cstdint>
#  include <map>
#  include <string>
#  include <utility>
#  include <vector>

#  include "opentelemetry/common/key_value_iterable_view.h"
#  include "opentelemetry/context/context.h"
#  include "opentelemetry/nostd/shared_ptr.h"
#  include "opentelemetry/nostd/utility.h"
#  include "opentelemetry/sdk/metrics/exemplar/filter_type.h"
#  include "opentelemetry/sdk/metrics/exemplar/reservoir.h"
#  include "opentelemetry/sdk/metrics/instruments.h"
#  include "opentelemetry/sdk/metrics/state/sync_metric_storage.h"
#  include "opentelemetry/sdk/metrics/view/attributes_processor.h"
#  include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
namespace
{

using AttributeMap = std::map<std::string, std::string>;

class DeterministicReservoir final : public ExemplarReservoir
{
public:
  explicit DeterministicReservoir(size_t accept_every) : accept_every_{accept_every} {}

  void OfferMeasurement(int64_t value,
                        const MetricAttributes &attributes,
                        const opentelemetry::context::Context & /* context */) noexcept override
  {
    if (ShouldAccept())
    {
      stored_attributes_ = attributes;
      benchmark::DoNotOptimize(value);
      benchmark::DoNotOptimize(stored_attributes_);
    }
  }

  void OfferMeasurement(double value,
                        const MetricAttributes &attributes,
                        const opentelemetry::context::Context & /* context */) noexcept override
  {
    if (ShouldAccept())
    {
      stored_attributes_ = attributes;
      benchmark::DoNotOptimize(value);
      benchmark::DoNotOptimize(stored_attributes_);
    }
  }

  void OfferMeasurement(int64_t value,
                        const opentelemetry::common::KeyValueIterable &attributes,
                        const opentelemetry::context::Context & /* context */) noexcept override
  {
    if (ShouldAccept())
    {
      MetricAttributes owned_attributes{attributes};
      stored_attributes_ = owned_attributes;
      benchmark::DoNotOptimize(value);
      benchmark::DoNotOptimize(stored_attributes_);
    }
  }

  void OfferMeasurement(double value,
                        const opentelemetry::common::KeyValueIterable &attributes,
                        const opentelemetry::context::Context & /* context */) noexcept override
  {
    if (ShouldAccept())
    {
      MetricAttributes owned_attributes{attributes};
      stored_attributes_ = owned_attributes;
      benchmark::DoNotOptimize(value);
      benchmark::DoNotOptimize(stored_attributes_);
    }
  }

  std::vector<std::shared_ptr<ExemplarData>> CollectAndReset(
      const MetricAttributes & /* point_attributes */) noexcept override
  {
    return {};
  }

  size_t GetOffersSeen() const noexcept { return offers_seen_; }

private:
  bool ShouldAccept() noexcept
  {
    ++offers_seen_;
    return accept_every_ != 0 && offers_seen_ % accept_every_ == 0;
  }

  size_t accept_every_;
  size_t offers_seen_ = 0;
  MetricAttributes stored_attributes_;
};

AttributeMap MakeAttributes(size_t attribute_count)
{
  AttributeMap attributes;
  for (size_t i = 0; i < attribute_count; ++i)
  {
    attributes.emplace("attribute-" + std::to_string(i),
                       std::string(32, static_cast<char>('a' + i % 26)));
  }
  return attributes;
}

void BenchmarkArguments(benchmark::internal::Benchmark *benchmark)
{
  constexpr int64_t kAttributeCounts[] = {0, 3, 10, 30};
  constexpr int64_t kAcceptEvery[]     = {0, 100, 10, 1};
  for (int64_t attribute_count : kAttributeCounts)
  {
    for (int64_t accept_every : kAcceptEvery)
    {
      benchmark->Args({attribute_count, accept_every});
    }
  }
}

void BM_RecordWithExemplarSelection(benchmark::State &state)
{
  const auto attribute_count = static_cast<size_t>(state.range(0));
  const auto accept_every    = static_cast<size_t>(state.range(1));
  auto attributes            = MakeAttributes(attribute_count);
  opentelemetry::common::KeyValueIterableView<AttributeMap> attribute_view{attributes};

  auto *reservoir = new DeterministicReservoir{accept_every};
  nostd::shared_ptr<ExemplarReservoir> reservoir_handle{reservoir};
  std::shared_ptr<DefaultAttributesProcessor> attributes_processor{
      new DefaultAttributesProcessor{}};
  InstrumentDescriptor instrument_descriptor{"benchmark.counter", "", "", InstrumentType::kCounter,
                                             InstrumentValueType::kDouble};
  SyncMetricStorage storage{instrument_descriptor,       AggregationType::kSum,
                            attributes_processor,        ExemplarFilterType::kAlwaysOn,
                            std::move(reservoir_handle), nullptr};

  for (auto _ : state)
  {
    storage.RecordDouble(7.0, attribute_view, opentelemetry::context::Context{});
  }

  benchmark::DoNotOptimize(reservoir->GetOffersSeen());
  state.SetItemsProcessed(state.iterations());
}

BENCHMARK(BM_RecordWithExemplarSelection)->Apply(BenchmarkArguments);

}  // namespace
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE

#endif  // ENABLE_METRICS_EXEMPLAR_PREVIEW

BENCHMARK_MAIN();
