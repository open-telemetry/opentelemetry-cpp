// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW

#  include <benchmark/benchmark.h>
#  include <cstdint>
#  include <string>
#  include <utility>

#  include "opentelemetry/context/context.h"
#  include "opentelemetry/nostd/shared_ptr.h"
#  include "opentelemetry/sdk/metrics/data/exemplar_data.h"
#  include "opentelemetry/sdk/metrics/exemplar/reservoir.h"
#  include "opentelemetry/sdk/metrics/exemplar/reservoir_cell.h"
#  include "opentelemetry/sdk/metrics/exemplar/simple_fixed_size_exemplar_reservoir.h"

namespace
{

using opentelemetry::context::Context;
using opentelemetry::nostd::shared_ptr;
using opentelemetry::sdk::metrics::ExemplarReservoir;
using opentelemetry::sdk::metrics::MetricAttributes;
using opentelemetry::sdk::metrics::ReservoirCell;
using opentelemetry::sdk::metrics::SimpleFixedSizeExemplarReservoir;

class SharedSimpleFixedSizeExemplarReservoirFixture : public benchmark::Fixture
{
public:
  using benchmark::Fixture::SetUp;
  using benchmark::Fixture::TearDown;

  void SetUp(benchmark::State &state) override
  {
    if (state.thread_index() != 0)
    {
      return;
    }

    auto selector = SimpleFixedSizeExemplarReservoir::GetSimpleFixedSizeCellSelector(1);
    reservoir_    = shared_ptr<ExemplarReservoir>{
        new SimpleFixedSizeExemplarReservoir{1, selector, &ReservoirCell::GetAndResetDouble}};

    // Model a reservoir partway through a normal collection interval. This
    // keeps the benchmark focused on the steady-state offer path.
    for (int64_t i = 0; i < 1024; ++i)
    {
      reservoir_->OfferMeasurement(static_cast<double>(i), attributes_, context_);
    }
  }

  void TearDown(benchmark::State &state) override
  {
    if (state.thread_index() == 0)
    {
      reservoir_ = nullptr;
    }
  }

protected:
  shared_ptr<ExemplarReservoir> reservoir_;
  MetricAttributes attributes_{{"http.request.method", "GET"},
                               {"http.route", "/checkout"},
                               {"http.response.status_code", int64_t{200}},
                               {"service.version", "1.42.0"}};
  Context context_;
};

BENCHMARK_DEFINE_F(SharedSimpleFixedSizeExemplarReservoirFixture, OfferMeasurement)
(benchmark::State &state)
{
  double value = 10.0 + static_cast<double>(state.thread_index());
  for (auto _ : state)
  {
    reservoir_->OfferMeasurement(value, attributes_, context_);
    value += 0.001;
  }

  benchmark::DoNotOptimize(value);
  state.SetItemsProcessed(state.iterations());
}

BENCHMARK_REGISTER_F(SharedSimpleFixedSizeExemplarReservoirFixture, OfferMeasurement)
    ->ThreadRange(1, 8)
    ->UseRealTime()
    ->Unit(benchmark::kNanosecond);

}  // namespace

BENCHMARK_MAIN();

#endif  // ENABLE_METRICS_EXEMPLAR_PREVIEW
