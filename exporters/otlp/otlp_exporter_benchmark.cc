#include "otlp_exporter.h"

#include <benchmark/benchmark.h>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

const int kBatchSize     = 200;
const int kNumAttributes = 5;

const trace::TraceId kTraceId(std::array<const uint8_t, trace::TraceId::kSize>(
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}));
const trace::SpanId kSpanId(std::array<const uint8_t, trace::SpanId::kSize>(
    {0, 0, 0, 0, 0, 0, 0, 2}));
const trace::SpanId kParentSpanId(std::array<const uint8_t, trace::SpanId::kSize>(
    {0, 0, 0, 0, 0, 0, 0, 3}));

// Create a fake service stub to avoid dependency on gmock
class FakeServiceStub : public proto::collector::trace::v1::TraceService::StubInterface
{
  grpc::Status Export(grpc::ClientContext *context,
                      const proto::collector::trace::v1::ExportTraceServiceRequest &request,
                      proto::collector::trace::v1::ExportTraceServiceResponse *response) override
  {
    (void)context;
    (void)request;
    (void)response;
    return grpc::Status::OK;
  }

  grpc::ClientAsyncResponseReaderInterface<proto::collector::trace::v1::ExportTraceServiceResponse>
      *AsyncExportRaw(grpc::ClientContext *context,
                      const proto::collector::trace::v1::ExportTraceServiceRequest &request,
                      grpc::CompletionQueue *cq) override
  {
    (void)context;
    (void)request;
    (void)cq;
    return nullptr;
  }

  grpc::ClientAsyncResponseReaderInterface<proto::collector::trace::v1::ExportTraceServiceResponse>
      *PrepareAsyncExportRaw(grpc::ClientContext *context,
                             const proto::collector::trace::v1::ExportTraceServiceRequest &request,
                             grpc::CompletionQueue *cq) override
  {
    (void)context;
    (void)request;
    (void)cq;
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

// Benchmark Export() with empty spans
void BM_OtlpExporterEmptySpans(benchmark::State &state)
{
  std::unique_ptr<OtlpExporterTestPeer> testpeer(new OtlpExporterTestPeer());
  auto exporter = testpeer->GetExporter();

  while (state.KeepRunning())
  {
    std::unique_ptr<sdk::trace::Recordable> batch_arr[kBatchSize];

    for (int i = 0; i < kBatchSize; i++)
    {
      auto recordable = exporter->MakeRecordable();
      batch_arr[i]    = std::move(recordable);
    }
    nostd::span<std::unique_ptr<sdk::trace::Recordable>> batch(batch_arr, kBatchSize);

    exporter->Export(batch);
  }
}
BENCHMARK(BM_OtlpExporterEmptySpans);

// Benchmark Export() with some span fields filled
void BM_OtlpExporterSparseSpans(benchmark::State &state)
{
  std::unique_ptr<OtlpExporterTestPeer> testpeer(new OtlpExporterTestPeer());
  auto exporter = testpeer->GetExporter();

  while (state.KeepRunning())
  {
    std::unique_ptr<sdk::trace::Recordable> batch_arr[kBatchSize];

    for (int i = 0; i < kBatchSize; i++)
    {
      auto recordable = exporter->MakeRecordable();

      recordable->SetIds(kTraceId, kSpanId, kParentSpanId);
      recordable->SetName("TestSpan");
      recordable->SetStartTime(core::SystemTimestamp(std::chrono::system_clock::now()));
      recordable->SetDuration(std::chrono::nanoseconds(10));

      batch_arr[i] = std::move(recordable);
    }
    nostd::span<std::unique_ptr<sdk::trace::Recordable>> batch(batch_arr, kBatchSize);

    exporter->Export(batch);
  }
}
BENCHMARK(BM_OtlpExporterSparseSpans);

// Benchmark Export() with all span fields filled

void BM_OtlpExporterDenseSpans(benchmark::State &state)
{
  std::unique_ptr<OtlpExporterTestPeer> testpeer(new OtlpExporterTestPeer());
  auto exporter = testpeer->GetExporter();

  while (state.KeepRunning())
  {
    std::unique_ptr<sdk::trace::Recordable> batch_arr[kBatchSize];

    for (int i = 0; i < kBatchSize; i++)
    {
      auto recordable = exporter->MakeRecordable();

      recordable->SetIds(kTraceId, kSpanId, kParentSpanId);
      recordable->SetName("TestSpan");
      recordable->SetStartTime(core::SystemTimestamp(std::chrono::system_clock::now()));
      recordable->SetDuration(std::chrono::nanoseconds(10));

      for (int i = 0; i < kNumAttributes; ++i)
      {
        recordable->SetAttribute("int_key_" + i, static_cast<int64_t>(i));
      }

      for (int i = 0; i < kNumAttributes; ++i)
      {
        recordable->SetAttribute("str_key_" + i, "string_val_" + std::to_string(i));
      }

      for (int i = 0; i < kNumAttributes; ++i)
      {
        recordable->SetAttribute("bool_key_" + i, true);
      }

      batch_arr[i] = std::move(recordable);
    }
    nostd::span<std::unique_ptr<sdk::trace::Recordable>> batch(batch_arr, kBatchSize);

    exporter->Export(batch);
  }
}
BENCHMARK(BM_OtlpExporterDenseSpans);

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE

BENCHMARK_MAIN();
