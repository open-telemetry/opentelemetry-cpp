// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

// clang-format off
//
// ~/build/sdk/test/trace/span_data_benchmark --benchmark_repetitions=5 --benchmark_display_aggregates_only=true
// 2026-07-01T15:59:10+00:00
// Running /home/devuser/build/sdk/test/trace/span_data_benchmark
// Run on (32 X 5700 MHz CPU s)
// CPU Caches:
//   L1 Data 48 KiB (x16)
//   L1 Instruction 32 KiB (x16)
//   L2 Unified 2048 KiB (x16)
//   L3 Unified 36864 KiB (x1)
// Load Average: 1.35, 1.52, 1.62
// ***WARNING*** ASLR is enabled, the results may have unreproducible noise in them.
// --------------------------------------------------------------------------------------------------------------
// Benchmark                                                                    Time             CPU   Iterations
// --------------------------------------------------------------------------------------------------------------
// SpanDataFixture/RecordMinimalSpan_mean                                     341 ns          343 ns            5
// SpanDataFixture/RecordMinimalSpan_median                                   341 ns          343 ns            5
// SpanDataFixture/RecordMinimalSpan_stddev                                 0.950 ns        0.988 ns            5
// SpanDataFixture/RecordMinimalSpan_cv                                      0.28 %          0.29 %             5
// SpanDataFixture/RecordNominalSpan_mean                                     581 ns          584 ns            5
// SpanDataFixture/RecordNominalSpan_median                                   588 ns          590 ns            5
// SpanDataFixture/RecordNominalSpan_stddev                                  15.3 ns         15.1 ns            5
// SpanDataFixture/RecordNominalSpan_cv                                      2.63 %          2.59 %             5
// SpanDataFixture/RecordSpanWithAttributes/attribute_count:1_mean            390 ns          394 ns            5 items_per_second=2.5415M/s
// SpanDataFixture/RecordSpanWithAttributes/attribute_count:1_median          391 ns          395 ns            5 items_per_second=2.53367M/s
// SpanDataFixture/RecordSpanWithAttributes/attribute_count:1_stddev         4.17 ns         4.40 ns            5 items_per_second=28.4487k/s
// SpanDataFixture/RecordSpanWithAttributes/attribute_count:1_cv             1.07 %          1.12 %             5 items_per_second=1.12%
// SpanDataFixture/RecordSpanWithAttributes/attribute_count:10_mean           705 ns          708 ns            5 items_per_second=14.1257M/s
// SpanDataFixture/RecordSpanWithAttributes/attribute_count:10_median         707 ns          711 ns            5 items_per_second=14.0739M/s
// SpanDataFixture/RecordSpanWithAttributes/attribute_count:10_stddev        6.35 ns         6.36 ns            5 items_per_second=127.942k/s
// SpanDataFixture/RecordSpanWithAttributes/attribute_count:10_cv            0.90 %          0.90 %             5 items_per_second=0.91%
// SpanDataFixture/RecordSpanWithAttributes/attribute_count:128_mean         7479 ns         7484 ns            5 items_per_second=17.1122M/s
// SpanDataFixture/RecordSpanWithAttributes/attribute_count:128_median       7539 ns         7543 ns            5 items_per_second=16.969M/s
// SpanDataFixture/RecordSpanWithAttributes/attribute_count:128_stddev        189 ns          189 ns            5 items_per_second=438.424k/s
// SpanDataFixture/RecordSpanWithAttributes/attribute_count:128_cv           2.53 %          2.53 %             5 items_per_second=2.56%
// SpanDataFixture/RecordSpanWithEvents/event_count:1_mean                    456 ns          459 ns            5 items_per_second=2.17751M/s
// SpanDataFixture/RecordSpanWithEvents/event_count:1_median                  456 ns          459 ns            5 items_per_second=2.178M/s
// SpanDataFixture/RecordSpanWithEvents/event_count:1_stddev                 2.89 ns         3.02 ns            5 items_per_second=14.3182k/s
// SpanDataFixture/RecordSpanWithEvents/event_count:1_cv                     0.63 %          0.66 %             5 items_per_second=0.66%
// SpanDataFixture/RecordSpanWithEvents/event_count:10_mean                  1584 ns         1587 ns            5 items_per_second=6.30345M/s
// SpanDataFixture/RecordSpanWithEvents/event_count:10_median                1580 ns         1583 ns            5 items_per_second=6.318M/s
// SpanDataFixture/RecordSpanWithEvents/event_count:10_stddev                24.3 ns         24.4 ns            5 items_per_second=95.4207k/s
// SpanDataFixture/RecordSpanWithEvents/event_count:10_cv                    1.54 %          1.54 %             5 items_per_second=1.51%
// SpanDataFixture/RecordSpanWithEvents/event_count:128_mean                16434 ns        16438 ns            5 items_per_second=7.78791M/s
// SpanDataFixture/RecordSpanWithEvents/event_count:128_median              16429 ns        16432 ns            5 items_per_second=7.78954M/s
// SpanDataFixture/RecordSpanWithEvents/event_count:128_stddev                211 ns          211 ns            5 items_per_second=100.089k/s
// SpanDataFixture/RecordSpanWithEvents/event_count:128_cv                   1.28 %          1.28 %             5 items_per_second=1.29%
// SpanDataFixture/RecordSpanWithLinks/link_count:1_mean                      435 ns          437 ns            5 items_per_second=2.29077M/s
// SpanDataFixture/RecordSpanWithLinks/link_count:1_median                    435 ns          436 ns            5 items_per_second=2.29368M/s
// SpanDataFixture/RecordSpanWithLinks/link_count:1_stddev                   2.12 ns         2.39 ns            5 items_per_second=12.5108k/s
// SpanDataFixture/RecordSpanWithLinks/link_count:1_cv                       0.49 %          0.55 %             5 items_per_second=0.55%
// SpanDataFixture/RecordSpanWithLinks/link_count:10_mean                    1368 ns         1371 ns            5 items_per_second=7.2967M/s
// SpanDataFixture/RecordSpanWithLinks/link_count:10_median                  1375 ns         1378 ns            5 items_per_second=7.25823M/s
// SpanDataFixture/RecordSpanWithLinks/link_count:10_stddev                  22.3 ns         22.4 ns            5 items_per_second=121.346k/s
// SpanDataFixture/RecordSpanWithLinks/link_count:10_cv                      1.63 %          1.63 %             5 items_per_second=1.66%
// SpanDataFixture/RecordSpanWithLinks/link_count:128_mean                  15304 ns        15308 ns            5 items_per_second=8.38477M/s
// SpanDataFixture/RecordSpanWithLinks/link_count:128_median                14933 ns        14937 ns            5 items_per_second=8.56932M/s
// SpanDataFixture/RecordSpanWithLinks/link_count:128_stddev                  930 ns          930 ns            5 items_per_second=474.386k/s
// SpanDataFixture/RecordSpanWithLinks/link_count:128_cv                     6.08 %          6.08 %             5 items_per_second=5.66%
//
// clang-format on

#include <benchmark/benchmark.h>

#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include "opentelemetry/exporters/memory/in_memory_span_exporter.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/instrumentationscope/instrumentation_scope.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/test_common/sdk/trace/test_utils.h"
#include "opentelemetry/trace/span.h"
#include "opentelemetry/trace/tracer.h"

namespace trace_sdk  = opentelemetry::sdk::trace;
namespace test_utils = opentelemetry::test_common;

namespace
{

class SpanDataFixture : public benchmark::Fixture
{
public:
  void SetUp(const benchmark::State &) override
  {
    auto exporter  = std::make_unique<opentelemetry::exporter::memory::InMemorySpanExporter>();
    auto processor = std::make_unique<test_utils::BufferingSpanProcessor>(std::move(exporter));
    provider_      = std::make_shared<trace_sdk::TracerProvider>(std::move(processor));
    tracer_        = provider_->GetTracer(test_utils::TestScope().GetName(),
                                          test_utils::TestScope().GetVersion(),
                                          test_utils::TestScope().GetSchemaURL());
    test_utils::InitializeSpanTestData();
  }

protected:
  std::shared_ptr<trace_sdk::TracerProvider> provider_;
  opentelemetry::nostd::shared_ptr<opentelemetry::trace::Tracer> tracer_;
};

}  // namespace

// Baseline: start a span and end it immediately with no attributes, links, or events.
BENCHMARK_DEFINE_F(SpanDataFixture, RecordMinimalSpan)(benchmark::State &state)
{
  for (auto _ : state)
  {
    auto span = test_utils::StartMinimalSpan(*tracer_);
    span->End();
    state.PauseTiming();
    provider_->ForceFlush();
    state.ResumeTiming();
  }
}
BENCHMARK_REGISTER_F(SpanDataFixture, RecordMinimalSpan)->Unit(benchmark::kNanosecond);

// RecordNominalSpan span: representative mix of attribute types passed at creation.
BENCHMARK_DEFINE_F(SpanDataFixture, RecordNominalSpan)(benchmark::State &state)
{
  for (auto _ : state)
  {
    auto span = test_utils::StartNominalSpan(*tracer_);
    span->End();
    state.PauseTiming();
    provider_->ForceFlush();
    state.ResumeTiming();
  }
}
BENCHMARK_REGISTER_F(SpanDataFixture, RecordNominalSpan)->Unit(benchmark::kNanosecond);

BENCHMARK_DEFINE_F(SpanDataFixture, RecordSpanWithAttributes)(benchmark::State &state)
{
  const int64_t attribute_count = state.range(0);
  const std::vector<test_utils::SpanAttribute> attributes =
      test_utils::MakeAttributes(attribute_count);
  for (auto _ : state)
  {
    auto span = test_utils::StartSpanWithAttributes(*tracer_, attributes);
    span->End();
    state.PauseTiming();
    provider_->ForceFlush();
    state.ResumeTiming();
  }
  state.SetItemsProcessed(state.iterations() * attribute_count);
}
BENCHMARK_REGISTER_F(SpanDataFixture, RecordSpanWithAttributes)
    ->ArgName("attribute_count")
    ->Arg(1)
    ->Arg(10)
    ->Arg(test_utils::kSpanAttributeLimit)
    ->Unit(benchmark::kNanosecond);

BENCHMARK_DEFINE_F(SpanDataFixture, RecordSpanWithEvents)(benchmark::State &state)
{
  const int64_t event_count = state.range(0);

  for (auto _ : state)
  {
    auto span = test_utils::StartSpanWithEvents(*tracer_, static_cast<std::size_t>(event_count));
    span->End();
    state.PauseTiming();
    provider_->ForceFlush();
    state.ResumeTiming();
  }
  state.SetItemsProcessed(state.iterations() * event_count);
}
BENCHMARK_REGISTER_F(SpanDataFixture, RecordSpanWithEvents)
    ->ArgName("event_count")
    ->Arg(1)
    ->Arg(10)
    ->Arg(test_utils::kSpanEventLimit)
    ->Unit(benchmark::kNanosecond);

BENCHMARK_DEFINE_F(SpanDataFixture, RecordSpanWithLinks)(benchmark::State &state)
{
  const int64_t link_count = state.range(0);
  const std::vector<test_utils::LinkEntry> link_entries =
      test_utils::MakeLinkEntries(static_cast<std::size_t>(link_count));

  for (auto _ : state)
  {
    auto span = test_utils::StartSpanWithLinks(*tracer_, link_entries);
    span->End();
    state.PauseTiming();
    provider_->ForceFlush();
    state.ResumeTiming();
  }
  state.SetItemsProcessed(state.iterations() * link_count);
}
BENCHMARK_REGISTER_F(SpanDataFixture, RecordSpanWithLinks)
    ->ArgName("link_count")
    ->Arg(1)
    ->Arg(10)
    ->Arg(test_utils::kSpanLinkLimit)
    ->Unit(benchmark::kNanosecond);

int main(int argc, char **argv)
{
  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();
  benchmark::Shutdown();
  return 0;
}
