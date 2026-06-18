// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

// clang-format off
//
// 2026-06-18T21:05:11+00:00
// Running /home/devuser/build/exporters/otlp/otlp_recordable_benchmark
// Run on (32 X 5175.97 MHz CPU s)
// CPU Caches:
//   L1 Data 48 KiB (x16)
//   L1 Instruction 32 KiB (x16)
//   L2 Unified 2048 KiB (x16)
//   L3 Unified 36864 KiB (x1)
// Load Average: 0.61, 1.30, 1.33
// -----------------------------------------------------------------------------------
// Benchmark                                         Time             CPU   Iterations
// -----------------------------------------------------------------------------------
// OtlpSpanFixture/Minimal_mean                    355 ns          357 ns            5
// OtlpSpanFixture/Minimal_median                  355 ns          358 ns            5
// OtlpSpanFixture/Minimal_stddev                0.489 ns        0.790 ns            5
// OtlpSpanFixture/Minimal_cv                     0.14 %          0.22 %             5
// OtlpSpanFixture/Nominal_mean                    617 ns          619 ns            5
// OtlpSpanFixture/Nominal_median                  617 ns          620 ns            5
// OtlpSpanFixture/Nominal_stddev                0.960 ns         1.25 ns            5
// OtlpSpanFixture/Nominal_cv                     0.16 %          0.20 %             5
// OtlpSpanFixture/MaxAttributes_mean             8014 ns         8016 ns            5 items_per_second=16.0018M/s
// OtlpSpanFixture/MaxAttributes_median           7737 ns         7739 ns            5 items_per_second=16.5389M/s
// OtlpSpanFixture/MaxAttributes_stddev            419 ns          418 ns            5 items_per_second=819.876k/s
// OtlpSpanFixture/MaxAttributes_cv               5.23 %          5.22 %             5 items_per_second=5.12%
// BM_PopulateRequest/span_count:1_mean          0.398 us        0.398 us            5 items_per_second=2.51517M/s
// BM_PopulateRequest/span_count:1_median        0.397 us        0.397 us            5 items_per_second=2.52167M/s
// BM_PopulateRequest/span_count:1_stddev        0.002 us        0.002 us            5 items_per_second=12.1068k/s
// BM_PopulateRequest/span_count:1_cv             0.48 %          0.48 %             5 items_per_second=0.48%
// BM_PopulateRequest/span_count:512_mean         26.3 us         26.3 us            5 items_per_second=19.4714M/s
// BM_PopulateRequest/span_count:512_median       26.3 us         26.3 us            5 items_per_second=19.4746M/s
// BM_PopulateRequest/span_count:512_stddev      0.078 us        0.077 us            5 items_per_second=56.7323k/s
// BM_PopulateRequest/span_count:512_cv           0.30 %          0.29 %             5 items_per_second=0.29%
//
// clang-format on

#include <benchmark/benchmark.h>

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
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
#include "opentelemetry/sdk/instrumentationscope/instrumentation_scope.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/processor.h"
#include "opentelemetry/sdk/trace/recordable.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"
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

namespace otlp      = opentelemetry::exporter::otlp;
namespace trace_sdk = opentelemetry::sdk::trace;
namespace trace_api = opentelemetry::trace;

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

// A span processor that queues completed recordables into an internal buffer
// without serialising or destroying them mirroring the batch processor.
class BufferingProcessor final : public trace_sdk::SpanProcessor
{
public:
  using Buffer = std::vector<std::unique_ptr<trace_sdk::Recordable>>;

  explicit BufferingProcessor(std::unique_ptr<trace_sdk::SpanExporter> exporter)
      : exporter_(std::move(exporter))
  {}

  std::unique_ptr<trace_sdk::Recordable> MakeRecordable() noexcept override
  {
    return exporter_->MakeRecordable();
  }

  void OnStart(trace_sdk::Recordable &, const opentelemetry::trace::SpanContext &) noexcept override
  {}

  void OnEnd(std::unique_ptr<trace_sdk::Recordable> &&span) noexcept override
  {
    buffer_.push_back(std::move(span));
  }

  bool ForceFlush(std::chrono::microseconds) noexcept override
  {
    buffer_.clear();
    return true;
  }
  bool Shutdown(std::chrono::microseconds) noexcept override { return true; }

private:
  std::unique_ptr<trace_sdk::SpanExporter> exporter_;
  Buffer buffer_;
};

const opentelemetry::sdk::resource::Resource &TestResource()
{
  static const auto resource = opentelemetry::sdk::resource::Resource::Create(
      {{"service.name", opentelemetry::nostd::string_view{"benchmark_service"}},
       {"service.version", opentelemetry::nostd::string_view{"1.0.0"}}});
  return resource;
}

const opentelemetry::sdk::instrumentationscope::InstrumentationScope &TestScope()
{
  static auto scope = opentelemetry::sdk::instrumentationscope::InstrumentationScope::Create(
      "benchmark_scope", "1.0.0", "https://opentelemetry.io/schemas/1.24.0",
      {{"scope.source", "benchmark"}});
  return *scope;
}

// Benchmark fixture: each benchmark function gets its own TracerProvider backed
// by a BufferingProcessor with a NullSpanExporter.
class OtlpSpanFixture : public benchmark::Fixture
{
public:
  void SetUp(benchmark::State &) override
  {
    auto processor = std::make_unique<BufferingProcessor>(std::make_unique<NullSpanExporter>());
    provider_      = std::make_shared<trace_sdk::TracerProvider>(std::move(processor));
    tracer_        = provider_->GetTracer(TestScope().GetName(), TestScope().GetVersion(),
                                          TestScope().GetSchemaURL());
  }

protected:
  std::shared_ptr<trace_sdk::TracerProvider> provider_;
  opentelemetry::nostd::shared_ptr<opentelemetry::trace::Tracer> tracer_;
};

// Benchmark argument values.
constexpr int kMaxAttributes = 128;  // default attribute count limit
constexpr int kBatchSmall    = 1;    // single span
constexpr int kBatchLarge    = 512;  // default batch size

// Pre-built keys for the max-attributes benchmark (matches SDK default attribute limit).
const std::vector<std::string> &MaxAttributeKeys()
{
  static const std::vector<std::string> keys = []() {
    std::vector<std::string> result;
    result.reserve(kMaxAttributes);
    for (int key_idx = 0; key_idx < kMaxAttributes; ++key_idx)
    {
      result.push_back("attribute." + std::to_string(key_idx));
    }
    return result;
  }();
  return keys;
}

// Build a batch of N fully-populated OtlpRecordable spans for PopulateRequest benchmarks.
std::vector<std::unique_ptr<opentelemetry::sdk::trace::Recordable>> MakeSpanBatch(
    std::size_t span_count)
{
  constexpr uint8_t kTraceIdBytes[opentelemetry::trace::TraceId::kSize] = {1, 2, 3, 4, 5, 6, 7, 8,
                                                                           1, 2, 3, 4, 5, 6, 7, 8};
  constexpr uint8_t kSpanIdBytes[opentelemetry::trace::SpanId::kSize]   = {1, 2, 3, 4, 5, 6, 7, 8};
  constexpr uint8_t kParentIdBytes[opentelemetry::trace::SpanId::kSize] = {8, 7, 6, 5, 4, 3, 2, 1};
  const opentelemetry::trace::TraceId trace_id{kTraceIdBytes};
  const opentelemetry::trace::SpanId span_id{kSpanIdBytes};
  const opentelemetry::trace::SpanId parent_id{kParentIdBytes};
  const opentelemetry::trace::SpanContext ctx{
      trace_id, span_id,
      opentelemetry::trace::TraceFlags{opentelemetry::trace::TraceFlags::kIsSampled}, true};
  const auto now = opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now());

  std::vector<std::unique_ptr<opentelemetry::sdk::trace::Recordable>> batch;
  batch.reserve(span_count);
  for (std::size_t span_idx = 0; span_idx < span_count; ++span_idx)
  {
    auto recordable = std::make_unique<otlp::OtlpRecordable>();
    recordable->SetIdentity(ctx, parent_id);
    recordable->SetName("benchmark_span");
    recordable->SetSpanKind(trace_api::SpanKind::kServer);
    recordable->SetStartTime(now);
    recordable->SetDuration(std::chrono::milliseconds(5));
    recordable->SetResource(TestResource());
    recordable->SetInstrumentationScope(TestScope());
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

// Baseline: start a span and end it immediately with no attributes.
BENCHMARK_DEFINE_F(OtlpSpanFixture, Minimal)(benchmark::State &state)
{
  for (auto _ : state)
  {
    auto span = tracer_->StartSpan("benchmark_span");
    span->End();
    state.PauseTiming();
    provider_->ForceFlush();
    state.ResumeTiming();
  }
}
BENCHMARK_REGISTER_F(OtlpSpanFixture, Minimal)->Unit(benchmark::kNanosecond);

// Nominal span: representative mix of attribute types passed at creation.
BENCHMARK_DEFINE_F(OtlpSpanFixture, Nominal)(benchmark::State &state)
{
  using SpanAttribute =
      std::pair<opentelemetry::nostd::string_view, opentelemetry::common::AttributeValue>;
  static const std::vector<SpanAttribute> kAttributes = {
      {"benchmark.string.short", opentelemetry::nostd::string_view{"abcdefgh"}},
      {"benchmark.string.medium",
       opentelemetry::nostd::string_view{"abcdefghijklmnopqrstuvwxyz012345"}},
      {"benchmark.string.long",
       opentelemetry::nostd::string_view{
           "abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz01"}},
      {"benchmark.int", opentelemetry::common::AttributeValue{static_cast<int64_t>(1)}},
      {"benchmark.double", opentelemetry::common::AttributeValue{3.14159265358979}},
      {"benchmark.bool", opentelemetry::common::AttributeValue{true}},
  };
  for (auto _ : state)
  {
    auto span = tracer_->StartSpan("benchmark_span", kAttributes);
    span->End();
    state.PauseTiming();
    provider_->ForceFlush();
    state.ResumeTiming();
  }
}
BENCHMARK_REGISTER_F(OtlpSpanFixture, Nominal)->Unit(benchmark::kNanosecond);

// Max-attributes span: fills the recordable to its attribute limit (128) with string values.
BENCHMARK_DEFINE_F(OtlpSpanFixture, MaxAttributes)(benchmark::State &state)
{
  using SpanAttribute =
      std::pair<opentelemetry::nostd::string_view, opentelemetry::common::AttributeValue>;
  static const std::vector<SpanAttribute> kAttributes = []() {
    const auto &keys = MaxAttributeKeys();
    std::vector<SpanAttribute> attributes;
    attributes.reserve(kMaxAttributes);
    for (const auto &key : keys)
    {
      attributes.emplace_back(key, opentelemetry::nostd::string_view{"value-string-attribute"});
    }
    return attributes;
  }();
  for (auto _ : state)
  {
    auto span = tracer_->StartSpan("benchmark_span", kAttributes);
    span->End();
    state.PauseTiming();
    provider_->ForceFlush();
    state.ResumeTiming();
  }
  state.SetItemsProcessed(state.iterations() * kMaxAttributes);
}
BENCHMARK_REGISTER_F(OtlpSpanFixture, MaxAttributes)->Unit(benchmark::kNanosecond);

// Benchmark the arena allocation + OtlpRecordableUtils::PopulateRequest for a
// batch of N spans. This mirrors the path in OtlpGrpcExporter::Export.
static void BM_PopulateRequest(benchmark::State &state)
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
BENCHMARK(BM_PopulateRequest)
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
