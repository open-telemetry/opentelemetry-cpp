// Copyright 2021, OpenTelemetry Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "opentelemetry/exporters/otlp/otlp_exporter.h"
#include "opentelemetry/exporters/otlp/recordable.h"

#include <benchmark/benchmark.h>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

const int kBatchSize     = 200;
const int kNumAttributes = 5;
const int kNumIterations = 1000;

const trace::TraceId kTraceId(std::array<const uint8_t, trace::TraceId::kSize>(
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}));
const trace::SpanId kSpanId(std::array<const uint8_t, trace::SpanId::kSize>({0, 0, 0, 0, 0, 0, 0,
                                                                             2}));
const trace::SpanId kParentSpanId(std::array<const uint8_t, trace::SpanId::kSize>({0, 0, 0, 0, 0, 0,
                                                                                   0, 3}));
const auto kTraceState = opentelemetry::trace::TraceState::GetDefault() -> Set("key1", "value");
const opentelemetry::trace::SpanContext kSpanContext{
    kTraceId, kSpanId,
    opentelemetry::trace::TraceFlags{opentelemetry::trace::TraceFlags::kIsSampled}, true,
    kTraceState};

// ----------------------- Helper classes and functions ------------------------

// Create a fake service stub to avoid dependency on gmock
class FakeServiceStub : public proto::collector::trace::v1::TraceService::StubInterface
{
  grpc::Status Export(grpc::ClientContext *,
                      const proto::collector::trace::v1::ExportTraceServiceRequest &,
                      proto::collector::trace::v1::ExportTraceServiceResponse *) override
  {
    return grpc::Status::OK;
  }

  grpc::ClientAsyncResponseReaderInterface<proto::collector::trace::v1::ExportTraceServiceResponse>
      *AsyncExportRaw(grpc::ClientContext *,
                      const proto::collector::trace::v1::ExportTraceServiceRequest &,
                      grpc::CompletionQueue *) override
  {
    return nullptr;
  }

  grpc::ClientAsyncResponseReaderInterface<proto::collector::trace::v1::ExportTraceServiceResponse>
      *PrepareAsyncExportRaw(grpc::ClientContext *,
                             const proto::collector::trace::v1::ExportTraceServiceRequest &,
                             grpc::CompletionQueue *) override
  {
    return nullptr;
  }
};

// OtlpExporterTestPeer is a friend class of OtlpExporter
class OtlpExporterTestPeer
{
public:
  std::unique_ptr<sdk::trace::SpanExporter> GetExporter()
  {
    auto mock_stub = new FakeServiceStub();
    std::unique_ptr<proto::collector::trace::v1::TraceService::StubInterface> stub_interface(
        mock_stub);
    return std::unique_ptr<sdk::trace::SpanExporter>(
        new exporter::otlp::OtlpExporter(std::move(stub_interface)));
  }
};

// Helper function to create empty spans
void CreateEmptySpans(std::array<std::unique_ptr<sdk::trace::Recordable>, kBatchSize> &recordables)
{
  for (int i = 0; i < kBatchSize; i++)
  {
    auto recordable = std::unique_ptr<sdk::trace::Recordable>(new Recordable);
    recordables[i]  = std::move(recordable);
  }
}

// Helper function to create sparse spans
void CreateSparseSpans(std::array<std::unique_ptr<sdk::trace::Recordable>, kBatchSize> &recordables)
{
  for (int i = 0; i < kBatchSize; i++)
  {
    auto recordable = std::unique_ptr<sdk::trace::Recordable>(new Recordable);

    recordable->SetIdentity(kSpanContext, kParentSpanId);
    recordable->SetName("TestSpan");
    recordable->SetStartTime(common::SystemTimestamp(std::chrono::system_clock::now()));
    recordable->SetDuration(std::chrono::nanoseconds(10));

    recordables[i] = std::move(recordable);
  }
}

// Helper function to create dense spans
void CreateDenseSpans(std::array<std::unique_ptr<sdk::trace::Recordable>, kBatchSize> &recordables)
{
  for (int i = 0; i < kBatchSize; i++)
  {
    auto recordable = std::unique_ptr<sdk::trace::Recordable>(new Recordable);

    recordable->SetIdentity(kSpanContext, kParentSpanId);
    recordable->SetName("TestSpan");
    recordable->SetStartTime(common::SystemTimestamp(std::chrono::system_clock::now()));
    recordable->SetDuration(std::chrono::nanoseconds(10));

    for (int i = 0; i < kNumAttributes; i++)
    {
      recordable->SetAttribute("int_key_" + i, static_cast<int64_t>(i));
      recordable->SetAttribute("str_key_" + i, "string_val_" + i);
      recordable->SetAttribute("bool_key_" + i, true);
    }

    recordables[i] = std::move(recordable);
  }
}

// ------------------------------ Benchmark tests ------------------------------

// Benchmark Export() with empty spans
void BM_OtlpExporterEmptySpans(benchmark::State &state)
{
  std::unique_ptr<OtlpExporterTestPeer> testpeer(new OtlpExporterTestPeer());
  auto exporter = testpeer->GetExporter();

  while (state.KeepRunningBatch(kNumIterations))
  {
    std::array<std::unique_ptr<sdk::trace::Recordable>, kBatchSize> recordables;
    CreateEmptySpans(recordables);
    exporter->Export(nostd::span<std::unique_ptr<sdk::trace::Recordable>>(recordables));
  }
}
BENCHMARK(BM_OtlpExporterEmptySpans);

// Benchmark Export() with sparse spans
void BM_OtlpExporterSparseSpans(benchmark::State &state)
{
  std::unique_ptr<OtlpExporterTestPeer> testpeer(new OtlpExporterTestPeer());
  auto exporter = testpeer->GetExporter();

  while (state.KeepRunningBatch(kNumIterations))
  {
    std::array<std::unique_ptr<sdk::trace::Recordable>, kBatchSize> recordables;
    CreateSparseSpans(recordables);
    exporter->Export(nostd::span<std::unique_ptr<sdk::trace::Recordable>>(recordables));
  }
}
BENCHMARK(BM_OtlpExporterSparseSpans);

// Benchmark Export() with dense spans
void BM_OtlpExporterDenseSpans(benchmark::State &state)
{
  std::unique_ptr<OtlpExporterTestPeer> testpeer(new OtlpExporterTestPeer());
  auto exporter = testpeer->GetExporter();

  while (state.KeepRunningBatch(kNumIterations))
  {
    std::array<std::unique_ptr<sdk::trace::Recordable>, kBatchSize> recordables;
    CreateDenseSpans(recordables);
    exporter->Export(nostd::span<std::unique_ptr<sdk::trace::Recordable>>(recordables));
  }
}
BENCHMARK(BM_OtlpExporterDenseSpans);

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE

BENCHMARK_MAIN();
