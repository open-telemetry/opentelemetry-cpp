// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

// Phase isolated benchmarks for the OTLP trace recordable.
//
// otlp_recordable_benchmark.cc measures a whole span in one timed region, which hides where the
// cost of recording into a protobuf message actually falls. These benchmarks time one phase of
// the application thread path at a time, so a change to the recordable can be attributed to the
// phase it affects:
//
//   StartSpan         Tracer::StartSpan, which creates the Recordable through the processor and
//                     records the identity, the name, the kind and the start time.
//   SetAttribute      Span::SetAttribute for every attribute of the span shape.
//   End               Span::End, which records the duration and hands the Recordable to the
//                     processor. This is the last work the application thread does for the span.
//   DestroyRecordable Destruction of the recorded Recordable. OtlpRecordable has no End method,
//                     and with a BatchSpanProcessor this cost is paid by the exporter thread
//                     after the export, not by the application thread. It is measured separately
//                     for that reason.
//
// Every case is run twice over, with one span alive at a time and with 128 spans alive at a
// time, because a recordable that owns an Arena holds a block of it for the whole life of the
// span and the two cases land in different parts of the allocator. One live span is what a
// simple processor gives, many live spans is what a batch processor queue gives.
//
// Only the phase under test is timed, through UseManualTime, so the reported Time column is the
// cost of the phase for the whole group of live spans. The per span cost, which is the number to
// read, is reported as the ns/span counter. Ignore the CPU column, which with manual timing
// covers the untimed setup and teardown as well.
//
// Building with OTELCPP_BENCHMARK_COUNT_ALLOCATIONS defined replaces the global operator new so
// every benchmark also reports the heap allocations the phase performs, as allocs/span and, for
// the attribute phase, allocs/attr. Timing and allocation counting are deliberately separate
// builds, since the counter perturbs the very allocations it measures.
//
// clang-format off
//
// ~/build/exporters/otlp/otlp_recordable_phase_benchmark --benchmark_repetitions=5 --benchmark_display_aggregates_only=true
//
// clang-format on

#include <benchmark/benchmark.h>

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "opentelemetry/exporters/otlp/otlp_recordable.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/common/exporter_utils.h"
#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/recordable.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/test_common/sdk/trace/test_utils.h"
#include "opentelemetry/trace/span.h"
#include "opentelemetry/trace/tracer.h"

#ifdef OTELCPP_BENCHMARK_COUNT_ALLOCATIONS
#  include <cstdlib>
#  include <new>
#endif

namespace otlp       = opentelemetry::exporter::otlp;
namespace trace_sdk  = opentelemetry::sdk::trace;
namespace test_utils = opentelemetry::test_common;

#ifdef OTELCPP_BENCHMARK_COUNT_ALLOCATIONS

// Counted per thread, so the allocations of the thread under test are not mixed with the
// allocations of the benchmark harness or of any exporter thread. A plain zero initialized
// thread_local of scalar type needs no lazy initialization, so reading it here cannot recurse
// back into operator new.
namespace
{
thread_local std::size_t g_allocation_count = 0;

void *CountedAllocate(std::size_t size)
{
  ++g_allocation_count;
  void *pointer = std::malloc(size != 0 ? size : 1);
  if (pointer == nullptr)
  {
    throw std::bad_alloc();
  }
  return pointer;
}

void *CountedAllocateNoThrow(std::size_t size) noexcept
{
  ++g_allocation_count;
  return std::malloc(size != 0 ? size : 1);
}

}  // namespace

void *operator new(std::size_t size)
{
  return CountedAllocate(size);
}
void *operator new[](std::size_t size)
{
  return CountedAllocate(size);
}
void *operator new(std::size_t size, const std::nothrow_t &) noexcept
{
  return CountedAllocateNoThrow(size);
}
void *operator new[](std::size_t size, const std::nothrow_t &) noexcept
{
  return CountedAllocateNoThrow(size);
}
void operator delete(void *pointer) noexcept
{
  std::free(pointer);
}
void operator delete[](void *pointer) noexcept
{
  std::free(pointer);
}
void operator delete(void *pointer, std::size_t) noexcept
{
  std::free(pointer);
}
void operator delete[](void *pointer, std::size_t) noexcept
{
  std::free(pointer);
}
void operator delete(void *pointer, const std::nothrow_t &) noexcept
{
  std::free(pointer);
}
void operator delete[](void *pointer, const std::nothrow_t &) noexcept
{
  std::free(pointer);
}

#endif  // OTELCPP_BENCHMARK_COUNT_ALLOCATIONS

namespace
{

std::size_t CurrentAllocationCount() noexcept
{
#ifdef OTELCPP_BENCHMARK_COUNT_ALLOCATIONS
  return g_allocation_count;
#else
  return 0;
#endif
}

constexpr bool kAllocationCountingEnabled =
#ifdef OTELCPP_BENCHMARK_COUNT_ALLOCATIONS
    true;
#else
    false;
#endif

// A no-op SpanExporter whose MakeRecordable creates an OtlpRecordable.
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

constexpr const char *kSpanName = "benchmark_span";

// How many spans the phase under test is run over before the batch is torn down again, which is
// also how many spans are alive at once. It matters: a recordable that owns an Arena holds one
// block of it for as long as the span lives, so a run that keeps many spans alive exercises a
// different part of the allocator than a run that records one span and releases it. Both are
// real, a simple processor releases immediately while a batch processor queues up to its whole
// queue, so both are measured.
constexpr std::int64_t kLiveSpansFew  = 1;
constexpr std::int64_t kLiveSpansMany = 128;

// A value comfortably past the small buffer of any common std::string implementation, so the
// character buffer of an arena backed string field is still a heap allocation. This is what the
// LongStrings shape isolates.
constexpr std::size_t kLongStringAttributeCount = 8;
constexpr std::size_t kLongStringValueLength    = 64;

// The span shapes the phases are measured over.
enum class SpanShape : int
{
  kMinimal     = 0,  // no attributes at all
  kNominal     = 1,  // the representative mix shared with otlp_recordable_benchmark.cc
  kLongStrings = 2,  // short keys, values past the std::string small buffer
};

const std::vector<test_utils::SpanAttribute> &LongStringAttributes()
{
  static const std::vector<std::string> kValues = []() {
    std::vector<std::string> values;
    values.reserve(kLongStringAttributeCount);
    for (std::size_t index = 0; index < kLongStringAttributeCount; ++index)
    {
      values.emplace_back(kLongStringValueLength, static_cast<char>('a' + (index % 26)));
    }
    return values;
  }();

  static const std::vector<test_utils::SpanAttribute> kAttributes = []() {
    const auto &keys = test_utils::AttributeKeys();
    std::vector<test_utils::SpanAttribute> attributes;
    attributes.reserve(kLongStringAttributeCount);
    for (std::size_t index = 0; index < kLongStringAttributeCount; ++index)
    {
      attributes.emplace_back(keys.at(index), opentelemetry::nostd::string_view{kValues.at(index)});
    }
    return attributes;
  }();

  return kAttributes;
}

const std::vector<test_utils::SpanAttribute> &ShapeAttributes(SpanShape shape)
{
  static const std::vector<test_utils::SpanAttribute> kNoAttributes;
  switch (shape)
  {
    case SpanShape::kNominal:
      return test_utils::NominalAttributes();
    case SpanShape::kLongStrings:
      return LongStringAttributes();
    case SpanShape::kMinimal:
    default:
      return kNoAttributes;
  }
}

const char *ShapeName(SpanShape shape)
{
  switch (shape)
  {
    case SpanShape::kNominal:
      return "nominal";
    case SpanShape::kLongStrings:
      return "long_strings";
    case SpanShape::kMinimal:
    default:
      return "minimal";
  }
}

using Clock = std::chrono::steady_clock;

double Seconds(Clock::time_point start, Clock::time_point end) noexcept
{
  return std::chrono::duration<double>(end - start).count();
}

// The pair of clock reads that brackets a timed phase costs a fixed amount, and with one span
// alive that cost is a visible part of a phase as short as End. It is measured once and taken
// off every iteration so the reported per span numbers are comparable across live span counts.
double ClockOverheadSeconds()
{
  static const double kOverhead = []() {
    constexpr int kSamples = 1024;
    double best            = 1.0;
    for (int sample = 0; sample < kSamples; ++sample)
    {
      const auto first  = Clock::now();
      const auto second = Clock::now();
      const double gap  = Seconds(first, second);
      if (gap < best)
      {
        best = gap;
      }
    }
    return best;
  }();
  return kOverhead;
}

// Elapsed time of a timed phase with the cost of the bracketing clock reads taken off.
double MeasuredSeconds(Clock::time_point start, Clock::time_point end) noexcept
{
  const double elapsed = Seconds(start, end) - ClockOverheadSeconds();
  return elapsed > 0.0 ? elapsed : 0.0;
}

// Reports the accumulated timings and allocation counts of a phase per single span, which is what
// every table in the issue is expressed in. The Time column stays the cost of a whole batch, so
// that the iteration count google benchmark derives from it matches the real wall time.
void ReportPhase(benchmark::State &state,
                 double total_seconds,
                 std::size_t total_allocations,
                 std::int64_t live_spans,
                 SpanShape shape)
{
  const auto span_count = static_cast<double>(live_spans);

  state.counters["ns/span"] =
      benchmark::Counter(total_seconds * 1e9 / span_count, benchmark::Counter::kAvgIterations);
  state.SetItemsProcessed(static_cast<std::int64_t>(state.iterations()) * live_spans);

  if (!kAllocationCountingEnabled)
  {
    return;
  }
  const double allocations_per_span = static_cast<double>(total_allocations) / span_count;
  state.counters["allocs/span"] =
      benchmark::Counter(allocations_per_span, benchmark::Counter::kAvgIterations);

  const std::size_t attribute_count = ShapeAttributes(shape).size();
  if (attribute_count != 0)
  {
    state.counters["allocs/attr"] =
        benchmark::Counter(allocations_per_span / static_cast<double>(attribute_count),
                           benchmark::Counter::kAvgIterations);
  }
}

// Label that tells the two axes of a case apart in the output.
std::string CaseLabel(std::int64_t live_spans, SpanShape shape)
{
  return std::string(ShapeName(shape)) + ", " + std::to_string(live_spans) + " live";
}

// Benchmark fixture: a TracerProvider backed by a BufferingSpanProcessor with a NullSpanExporter,
// so recordables are kept alive until ForceFlush and their destruction can be timed on its own.
class OtlpRecordablePhaseFixture : public benchmark::Fixture
{
public:
  using benchmark::Fixture::SetUp;

  void SetUp(benchmark::State &) override
  {
    auto processor =
        std::make_unique<test_utils::BufferingSpanProcessor>(std::make_unique<NullSpanExporter>());
    provider_ = std::make_shared<trace_sdk::TracerProvider>(std::move(processor));
    tracer_   = provider_->GetTracer(test_utils::TestScope().GetName(),
                                     test_utils::TestScope().GetVersion(),
                                     test_utils::TestScope().GetSchemaURL());
    test_utils::InitializeSpanTestData();
    LongStringAttributes();
    ClockOverheadSeconds();
    spans_.reserve(static_cast<std::size_t>(kLiveSpansMany));
  }

protected:
  void StartSpans(std::int64_t live_spans)
  {
    for (std::int64_t index = 0; index < live_spans; ++index)
    {
      spans_.push_back(tracer_->StartSpan(kSpanName));
    }
  }

  void SetAttributes(const std::vector<test_utils::SpanAttribute> &attributes)
  {
    for (auto &span : spans_)
    {
      for (const auto &attribute : attributes)
      {
        span->SetAttribute(attribute.first, attribute.second);
      }
    }
  }

  void EndSpans()
  {
    for (auto &span : spans_)
    {
      span->End();
    }
  }

  // Drops the span handles and destroys every recordable the processor has buffered.
  void DestroyRecordables()
  {
    spans_.clear();
    provider_->ForceFlush();
  }

  std::shared_ptr<trace_sdk::TracerProvider> provider_;
  opentelemetry::nostd::shared_ptr<opentelemetry::trace::Tracer> tracer_;
  std::vector<opentelemetry::nostd::shared_ptr<opentelemetry::trace::Span>> spans_;
};

}  // namespace

// Applies the live span count and the shape arguments every phase benchmark takes.
#define OTLP_PHASE_ARGS(bench)                                            \
  BENCHMARK_REGISTER_F(OtlpRecordablePhaseFixture, bench)                 \
      ->ArgNames({"live_spans", "shape"})                                 \
      ->Args({kLiveSpansFew, static_cast<int>(SpanShape::kMinimal)})      \
      ->Args({kLiveSpansFew, static_cast<int>(SpanShape::kNominal)})      \
      ->Args({kLiveSpansFew, static_cast<int>(SpanShape::kLongStrings)})  \
      ->Args({kLiveSpansMany, static_cast<int>(SpanShape::kMinimal)})     \
      ->Args({kLiveSpansMany, static_cast<int>(SpanShape::kNominal)})     \
      ->Args({kLiveSpansMany, static_cast<int>(SpanShape::kLongStrings)}) \
      ->UseManualTime()                                                   \
      ->Unit(benchmark::kNanosecond)

// StartSpan only. Creates the Recordable through the processor and records the identity, the
// name, the kind and the start time. The shape argument does not change what this phase does,
// since attributes are recorded by the next phase, but it is kept so every phase of a given
// shape can be read off one row of the table.
BENCHMARK_DEFINE_F(OtlpRecordablePhaseFixture, StartSpan)(benchmark::State &state)
{
  const auto live_spans = state.range(0);
  const auto shape      = static_cast<SpanShape>(state.range(1));
  state.SetLabel(CaseLabel(live_spans, shape));

  std::size_t allocations = 0;
  double seconds          = 0.0;
  for (auto _ : state)
  {
    const std::size_t allocations_before = CurrentAllocationCount();
    const auto start                     = Clock::now();
    StartSpans(live_spans);
    const auto end = Clock::now();
    allocations += CurrentAllocationCount() - allocations_before;

    EndSpans();
    DestroyRecordables();
    const double elapsed = MeasuredSeconds(start, end);
    seconds += elapsed;
    state.SetIterationTime(elapsed);
  }
  ReportPhase(state, seconds, allocations, live_spans, SpanShape::kMinimal);
}
OTLP_PHASE_ARGS(StartSpan);

// SetAttribute only, for every attribute of the shape. The minimal shape records no attributes,
// so this phase does not run for it and its cost is zero by construction. It is not registered,
// because timing an empty loop would never reach the minimum time google benchmark asks for.
BENCHMARK_DEFINE_F(OtlpRecordablePhaseFixture, SetAttribute)(benchmark::State &state)
{
  const auto live_spans  = state.range(0);
  const auto shape       = static_cast<SpanShape>(state.range(1));
  const auto &attributes = ShapeAttributes(shape);
  state.SetLabel(CaseLabel(live_spans, shape));

  std::size_t allocations = 0;
  double seconds          = 0.0;
  for (auto _ : state)
  {
    StartSpans(live_spans);

    const std::size_t allocations_before = CurrentAllocationCount();
    const auto start                     = Clock::now();
    SetAttributes(attributes);
    const auto end = Clock::now();
    allocations += CurrentAllocationCount() - allocations_before;

    EndSpans();
    DestroyRecordables();
    const double elapsed = MeasuredSeconds(start, end);
    seconds += elapsed;
    state.SetIterationTime(elapsed);
  }
  ReportPhase(state, seconds, allocations, live_spans, shape);
}
BENCHMARK_REGISTER_F(OtlpRecordablePhaseFixture, SetAttribute)
    ->ArgNames({"live_spans", "shape"})
    ->Args({kLiveSpansFew, static_cast<int>(SpanShape::kNominal)})
    ->Args({kLiveSpansFew, static_cast<int>(SpanShape::kLongStrings)})
    ->Args({kLiveSpansMany, static_cast<int>(SpanShape::kNominal)})
    ->Args({kLiveSpansMany, static_cast<int>(SpanShape::kLongStrings)})
    ->UseManualTime()
    ->Unit(benchmark::kNanosecond);

// End only. Records the duration and hands the Recordable to the processor. This is the last
// work the application thread does for the span.
BENCHMARK_DEFINE_F(OtlpRecordablePhaseFixture, End)(benchmark::State &state)
{
  const auto live_spans  = state.range(0);
  const auto shape       = static_cast<SpanShape>(state.range(1));
  const auto &attributes = ShapeAttributes(shape);
  state.SetLabel(CaseLabel(live_spans, shape));

  std::size_t allocations = 0;
  double seconds          = 0.0;
  for (auto _ : state)
  {
    StartSpans(live_spans);
    SetAttributes(attributes);

    const std::size_t allocations_before = CurrentAllocationCount();
    const auto start                     = Clock::now();
    EndSpans();
    const auto end = Clock::now();
    allocations += CurrentAllocationCount() - allocations_before;

    DestroyRecordables();
    const double elapsed = MeasuredSeconds(start, end);
    seconds += elapsed;
    state.SetIterationTime(elapsed);
  }
  ReportPhase(state, seconds, allocations, live_spans, shape);
}
OTLP_PHASE_ARGS(End);

// Destruction of the recorded Recordable, which OtlpRecordable has no End method for. With a
// BatchSpanProcessor this runs on the exporter thread after the export, so it is not part of the
// application thread cost of the three phases above.
BENCHMARK_DEFINE_F(OtlpRecordablePhaseFixture, DestroyRecordable)(benchmark::State &state)
{
  const auto live_spans  = state.range(0);
  const auto shape       = static_cast<SpanShape>(state.range(1));
  const auto &attributes = ShapeAttributes(shape);
  state.SetLabel(CaseLabel(live_spans, shape));

  std::size_t allocations = 0;
  double seconds          = 0.0;
  for (auto _ : state)
  {
    StartSpans(live_spans);
    SetAttributes(attributes);
    EndSpans();

    const std::size_t allocations_before = CurrentAllocationCount();
    const auto start                     = Clock::now();
    DestroyRecordables();
    const auto end = Clock::now();
    allocations += CurrentAllocationCount() - allocations_before;

    const double elapsed = MeasuredSeconds(start, end);
    seconds += elapsed;
    state.SetIterationTime(elapsed);
  }
  ReportPhase(state, seconds, allocations, live_spans, shape);
}
OTLP_PHASE_ARGS(DestroyRecordable);

int main(int argc, char **argv)
{
  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();
  benchmark::Shutdown();
  return 0;
}
