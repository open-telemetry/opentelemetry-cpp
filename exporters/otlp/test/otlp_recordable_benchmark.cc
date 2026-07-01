// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

// clang-format off
//
// ~/build/exporters/otlp/otlp_recordable_benchmark --benchmark_repetitions=5 --benchmark_display_aggregates_only=true
// 2026-07-01T15:57:22+00:00
// Running /home/devuser/build/exporters/otlp/otlp_recordable_benchmark
// Run on (32 X 5700 MHz CPU s)
// CPU Caches:
//   L1 Data 48 KiB (x16)
//   L1 Instruction 32 KiB (x16)
//   L2 Unified 2048 KiB (x16)
//   L3 Unified 36864 KiB (x1)
// Load Average: 2.05, 1.65, 1.67
// ***WARNING*** ASLR is enabled, the results may have unreproducible noise in them.
// --------------------------------------------------------------------------------------------------------------------
// Benchmark                                                                          Time             CPU   Iterations
// --------------------------------------------------------------------------------------------------------------------
// OtlpRecordableFixture/RecordMinimalSpan_mean                                     362 ns          364 ns            5
// OtlpRecordableFixture/RecordMinimalSpan_median                                   363 ns          365 ns            5
// OtlpRecordableFixture/RecordMinimalSpan_stddev                                  1.77 ns         1.97 ns            5
// OtlpRecordableFixture/RecordMinimalSpan_cv                                      0.49 %          0.54 %             5
// OtlpRecordableFixture/RecordNominalSpan_mean                                     638 ns          639 ns            5
// OtlpRecordableFixture/RecordNominalSpan_median                                   636 ns          637 ns            5
// OtlpRecordableFixture/RecordNominalSpan_stddev                                  8.01 ns         8.04 ns            5
// OtlpRecordableFixture/RecordNominalSpan_cv                                      1.26 %          1.26 %             5
// OtlpRecordableFixture/RecordSpanWithAttributes/attribute_count:1_mean            414 ns          416 ns            5 items_per_second=2.40287M/s
// OtlpRecordableFixture/RecordSpanWithAttributes/attribute_count:1_median          414 ns          416 ns            5 items_per_second=2.40294M/s
// OtlpRecordableFixture/RecordSpanWithAttributes/attribute_count:1_stddev         1.94 ns         2.14 ns            5 items_per_second=12.3138k/s
// OtlpRecordableFixture/RecordSpanWithAttributes/attribute_count:1_cv             0.47 %          0.51 %             5 items_per_second=0.51%
// OtlpRecordableFixture/RecordSpanWithAttributes/attribute_count:10_mean           880 ns          881 ns            5 items_per_second=11.3516M/s
// OtlpRecordableFixture/RecordSpanWithAttributes/attribute_count:10_median         883 ns          883 ns            5 items_per_second=11.3193M/s
// OtlpRecordableFixture/RecordSpanWithAttributes/attribute_count:10_stddev        12.8 ns         13.0 ns            5 items_per_second=167.444k/s
// OtlpRecordableFixture/RecordSpanWithAttributes/attribute_count:10_cv            1.46 %          1.47 %             5 items_per_second=1.48%
// OtlpRecordableFixture/RecordSpanWithAttributes/attribute_count:128_mean         7293 ns         7297 ns            5 items_per_second=17.7189M/s
// OtlpRecordableFixture/RecordSpanWithAttributes/attribute_count:128_median       6834 ns         6839 ns            5 items_per_second=18.7175M/s
// OtlpRecordableFixture/RecordSpanWithAttributes/attribute_count:128_stddev        842 ns          841 ns            5 items_per_second=1.922M/s
// OtlpRecordableFixture/RecordSpanWithAttributes/attribute_count:128_cv          11.55 %         11.53 %             5 items_per_second=10.85%
// OtlpRecordableFixture/RecordSpanWithEvents/event_count:1_mean                    502 ns          503 ns            5 items_per_second=1.98937M/s
// OtlpRecordableFixture/RecordSpanWithEvents/event_count:1_median                  501 ns          503 ns            5 items_per_second=1.99001M/s
// OtlpRecordableFixture/RecordSpanWithEvents/event_count:1_stddev                 1.04 ns        0.917 ns            5 items_per_second=3.63084k/s
// OtlpRecordableFixture/RecordSpanWithEvents/event_count:1_cv                     0.21 %          0.18 %             5 items_per_second=0.18%
// OtlpRecordableFixture/RecordSpanWithEvents/event_count:10_mean                  1705 ns         1707 ns            5 items_per_second=5.86103M/s
// OtlpRecordableFixture/RecordSpanWithEvents/event_count:10_median                1699 ns         1700 ns            5 items_per_second=5.88169M/s
// OtlpRecordableFixture/RecordSpanWithEvents/event_count:10_stddev                29.0 ns         29.0 ns            5 items_per_second=99.4244k/s
// OtlpRecordableFixture/RecordSpanWithEvents/event_count:10_cv                    1.70 %          1.70 %             5 items_per_second=1.70%
// OtlpRecordableFixture/RecordSpanWithEvents/event_count:128_mean                16750 ns        16750 ns            5 items_per_second=7.64354M/s
// OtlpRecordableFixture/RecordSpanWithEvents/event_count:128_median              16757 ns        16756 ns            5 items_per_second=7.63925M/s
// OtlpRecordableFixture/RecordSpanWithEvents/event_count:128_stddev                287 ns          287 ns            5 items_per_second=130.651k/s
// OtlpRecordableFixture/RecordSpanWithEvents/event_count:128_cv                   1.71 %          1.71 %             5 items_per_second=1.71%
// OtlpRecordableFixture/RecordSpanWithLinks/link_count:1_mean                      511 ns          513 ns            5 items_per_second=1.94877M/s
// OtlpRecordableFixture/RecordSpanWithLinks/link_count:1_median                    510 ns          512 ns            5 items_per_second=1.95458M/s
// OtlpRecordableFixture/RecordSpanWithLinks/link_count:1_stddev                   5.76 ns         6.17 ns            5 items_per_second=23.304k/s
// OtlpRecordableFixture/RecordSpanWithLinks/link_count:1_cv                       1.13 %          1.20 %             5 items_per_second=1.20%
// OtlpRecordableFixture/RecordSpanWithLinks/link_count:10_mean                    1784 ns         1786 ns            5 items_per_second=5.60068M/s
// OtlpRecordableFixture/RecordSpanWithLinks/link_count:10_median                  1785 ns         1787 ns            5 items_per_second=5.59477M/s
// OtlpRecordableFixture/RecordSpanWithLinks/link_count:10_stddev                  6.08 ns         6.23 ns            5 items_per_second=19.5651k/s
// OtlpRecordableFixture/RecordSpanWithLinks/link_count:10_cv                      0.34 %          0.35 %             5 items_per_second=0.35%
// OtlpRecordableFixture/RecordSpanWithLinks/link_count:128_mean                  19515 ns        19517 ns            5 items_per_second=6.62769M/s
// OtlpRecordableFixture/RecordSpanWithLinks/link_count:128_median                18663 ns        18666 ns            5 items_per_second=6.85722M/s
// OtlpRecordableFixture/RecordSpanWithLinks/link_count:128_stddev                 2403 ns         2403 ns            5 items_per_second=703.536k/s
// OtlpRecordableFixture/RecordSpanWithLinks/link_count:128_cv                    12.31 %         12.31 %             5 items_per_second=10.62%
// BM_OtlpPopulateRequest/span_count:1_mean                                       0.387 us        0.387 us            5 items_per_second=2.58274M/s
// BM_OtlpPopulateRequest/span_count:1_median                                     0.387 us        0.387 us            5 items_per_second=2.58201M/s
// BM_OtlpPopulateRequest/span_count:1_stddev                                     0.004 us        0.004 us            5 items_per_second=23.6738k/s
// BM_OtlpPopulateRequest/span_count:1_cv                                          0.92 %          0.92 %             5 items_per_second=0.92%
// BM_OtlpPopulateRequest/span_count:512_mean                                      23.6 us         23.6 us            5 items_per_second=21.7343M/s
// BM_OtlpPopulateRequest/span_count:512_median                                    23.5 us         23.5 us            5 items_per_second=21.7479M/s
// BM_OtlpPopulateRequest/span_count:512_stddev                                   0.260 us        0.259 us            5 items_per_second=238.588k/s
// BM_OtlpPopulateRequest/span_count:512_cv                                        1.10 %          1.10 %             5 items_per_second=1.10%
//
// clang-format on

#include <benchmark/benchmark.h>

#include <chrono>
#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/common/timestamp.h"
#include "opentelemetry/exporters/otlp/otlp_recordable.h"
#include "opentelemetry/exporters/otlp/otlp_recordable_utils.h"
#include "opentelemetry/exporters/otlp/protobuf_include_prefix.h"
#include "opentelemetry/exporters/otlp/protobuf_include_suffix.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/utility.h"
#include "opentelemetry/sdk/common/exporter_utils.h"
#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/recordable.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/test_common/sdk/trace/test_utils.h"
#include "opentelemetry/trace/span.h"
#include "opentelemetry/trace/span_context.h"
#include "opentelemetry/trace/span_id.h"
#include "opentelemetry/trace/span_metadata.h"
#include "opentelemetry/trace/trace_flags.h"
#include "opentelemetry/trace/trace_id.h"
#include "opentelemetry/trace/tracer.h"

// clang-format off
#include "opentelemetry/exporters/otlp/protobuf_include_prefix.h"  // IWYU pragma: keep
#include "google/protobuf/arena.h"
#include "opentelemetry/proto/collector/trace/v1/trace_service.pb.h"
#include "opentelemetry/exporters/otlp/protobuf_include_suffix.h"  // IWYU pragma: keep
// clang-format on

namespace otlp       = opentelemetry::exporter::otlp;
namespace trace_sdk  = opentelemetry::sdk::trace;
namespace trace_api  = opentelemetry::trace;
namespace test_utils = opentelemetry::test_common;

namespace
{

// A no-op SpanExporter whose MakeRecordable creates an OtlpRecordable.
// This exercises the full OTLP recordable population path via the public API.
class NullSpanExporter final : public trace_sdk::SpanExporter
{
public:
  std::unique_ptr<trace_sdk::Recordable> MakeRecordable() noexcept override
  {
    return std::make_unique<otlp::OtlpRecordable>();
  }

  opentelemetry::sdk::common::ExportResult Export(
      const opentelemetry::nostd::span<std::unique_ptr<trace_sdk::Recordable>> &) noexcept override
  {
    return opentelemetry::sdk::common::ExportResult::kSuccess;
  }

  bool ForceFlush(std::chrono::microseconds) noexcept override { return true; }

  bool Shutdown(std::chrono::microseconds) noexcept override { return true; }
};

// Benchmark fixture: each benchmark function gets its own TracerProvider backed
// by a BufferingSpanProcessor with a NullSpanExporter.
class OtlpRecordableFixture : public benchmark::Fixture
{
public:
  void SetUp(benchmark::State &) override
  {
    auto processor =
        std::make_unique<test_utils::BufferingSpanProcessor>(std::make_unique<NullSpanExporter>());
    provider_ = std::make_shared<trace_sdk::TracerProvider>(std::move(processor));
    tracer_   = provider_->GetTracer(test_utils::TestScope().GetName(),
                                     test_utils::TestScope().GetVersion(),
                                     test_utils::TestScope().GetSchemaURL());
    test_utils::InitializeSpanTestData();
  }

protected:
  std::shared_ptr<trace_sdk::TracerProvider> provider_;
  opentelemetry::nostd::shared_ptr<opentelemetry::trace::Tracer> tracer_;
};

// Benchmark argument values.
constexpr std::size_t kBatchSmall = 1;    // single span
constexpr std::size_t kBatchLarge = 512;  // default batch size

// Build a batch of N fully-populated OtlpRecordable spans for PopulateRequest benchmarks.
std::vector<std::unique_ptr<opentelemetry::sdk::trace::Recordable>> MakeSpanBatch(
    std::size_t span_count)
{
  const auto now = opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now());

  std::vector<std::unique_ptr<opentelemetry::sdk::trace::Recordable>> batch;
  batch.reserve(span_count);
  for (std::size_t span_idx = 0; span_idx < span_count; ++span_idx)
  {
    auto recordable = std::make_unique<otlp::OtlpRecordable>();
    recordable->SetIdentity(test_utils::TestSpanContext(), opentelemetry::trace::SpanId{});
    recordable->SetName("benchmark_span");
    recordable->SetSpanKind(trace_api::SpanKind::kServer);
    recordable->SetStartTime(now);
    recordable->SetDuration(std::chrono::milliseconds(5));
    recordable->SetResource(test_utils::TestResource());
    recordable->SetInstrumentationScope(test_utils::TestScope());
    batch.push_back(std::move(recordable));
  }
  return batch;
}

std::unique_ptr<google::protobuf::Arena> CreateArena()
{
  google::protobuf::ArenaOptions options;
  options.initial_block_size = 1024;
  options.max_block_size     = 65536;
  return std::make_unique<google::protobuf::Arena>(options);
}

}  // namespace

// Baseline: start a span and end it immediately with no attributes, links, or events.
BENCHMARK_DEFINE_F(OtlpRecordableFixture, RecordMinimalSpan)(benchmark::State &state)
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
BENCHMARK_REGISTER_F(OtlpRecordableFixture, RecordMinimalSpan)->Unit(benchmark::kNanosecond);

// RecordNominalSpan span: starts a span with a representative mix of attribute types then ends the
// span.
BENCHMARK_DEFINE_F(OtlpRecordableFixture, RecordNominalSpan)(benchmark::State &state)
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
BENCHMARK_REGISTER_F(OtlpRecordableFixture, RecordNominalSpan)->Unit(benchmark::kNanosecond);

// RecordSpanWithAttributes: starts a span with N attributes (all string values) then ends the span.
BENCHMARK_DEFINE_F(OtlpRecordableFixture, RecordSpanWithAttributes)(benchmark::State &state)
{
  const std::size_t attribute_count = static_cast<std::size_t>(state.range(0));
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
BENCHMARK_REGISTER_F(OtlpRecordableFixture, RecordSpanWithAttributes)
    ->ArgName("attribute_count")
    ->Arg(1)
    ->Arg(10)
    ->Arg(test_utils::kSpanAttributeLimit)
    ->Unit(benchmark::kNanosecond);

// RecordSpanWithEvents: starts a span via the tracer API with N events, then ends the span.
BENCHMARK_DEFINE_F(OtlpRecordableFixture, RecordSpanWithEvents)(benchmark::State &state)
{
  const int64_t event_count = state.range(0);

  for (auto _ : state)
  {
    auto span = test_utils::StartSpanWithEvents(*tracer_, event_count);
    span->End();
    state.PauseTiming();
    provider_->ForceFlush();
    state.ResumeTiming();
  }
  state.SetItemsProcessed(state.iterations() * event_count);
}
BENCHMARK_REGISTER_F(OtlpRecordableFixture, RecordSpanWithEvents)
    ->ArgName("event_count")
    ->Arg(1)
    ->Arg(10)
    ->Arg(test_utils::kSpanEventLimit)
    ->Unit(benchmark::kNanosecond);

// RecordSpanWithLinks: starts a span via the tracer API with N links, then ends the span.
BENCHMARK_DEFINE_F(OtlpRecordableFixture, RecordSpanWithLinks)(benchmark::State &state)
{
  const std::size_t link_count                          = static_cast<std::size_t>(state.range(0));
  const std::vector<test_utils::LinkEntry> link_entries = test_utils::MakeLinkEntries(link_count);

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
BENCHMARK_REGISTER_F(OtlpRecordableFixture, RecordSpanWithLinks)
    ->ArgName("link_count")
    ->Arg(1)
    ->Arg(10)
    ->Arg(test_utils::kSpanLinkLimit)
    ->Unit(benchmark::kNanosecond);

// Benchmark the arena allocation + OtlpRecordableUtils::PopulateRequest for a
// batch of N spans. This mirrors the path in OtlpGrpcExporter::Export.
static void BM_OtlpPopulateRequest(benchmark::State &state)
{
  const std::size_t span_count = static_cast<std::size_t>(state.range(0));
  auto batch                   = MakeSpanBatch(span_count);
  for (auto _ : state)
  {
    auto arena    = CreateArena();
    auto *request = google::protobuf::Arena::Create<
        opentelemetry::proto::collector::trace::v1::ExportTraceServiceRequest>(arena.get());

    opentelemetry::exporter::otlp::OtlpRecordableUtils::PopulateRequest(
        opentelemetry::nostd::span<std::unique_ptr<opentelemetry::sdk::trace::Recordable>>{
            batch.data(), batch.size()},
        request);
    benchmark::DoNotOptimize(request);
  }
  state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_OtlpPopulateRequest)
    ->ArgName("span_count")
    ->Arg(kBatchSmall)
    ->Arg(kBatchLarge)
    ->Unit(benchmark::kMicrosecond);

int main(int argc, char **argv)
{
  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();
  benchmark::Shutdown();
  return 0;
}
