#include "otlp_exporter.h"

#include <benchmark/benchmark.h>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

const int kBatchSize = 200;

const trace::TraceId kTraceId(std::array<const uint8_t, trace::TraceId::kSize>(
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}));
const trace::SpanId kSpanId(std::array<const uint8_t, trace::SpanId::kSize>(
    {0, 0, 0, 0, 0, 0, 0, 2}));
const trace::SpanId kParentSpanId(std::array<const uint8_t, trace::SpanId::kSize>(
    {0, 0, 0, 0, 0, 0, 0, 3}));

// OtlpExporterTestPeer is a friend class of OtlpExporter
class OtlpExporterTestPeer
{
public:
  std::unique_ptr<sdk::trace::SpanExporter> GetExporter(
      std::unique_ptr<proto::collector::trace::v1::TraceService::StubInterface> &stub_interface)
  {
    return std::unique_ptr<sdk::trace::SpanExporter>(
        new exporter::otlp::OtlpExporter(std::move(stub_interface)));
  }
};

class FakeServiceStub : public proto::collector::trace::v1::TraceService::StubInterface
{
  grpc::Status Export(grpc::ClientContext *context,
                      const proto::collector::trace::v1::ExportTraceServiceRequest &request,
                      proto::collector::trace::v1::ExportTraceServiceResponse *response) override
  {
    return grpc::Status::OK;
  }

  grpc::ClientAsyncResponseReaderInterface<proto::collector::trace::v1::ExportTraceServiceResponse>
      *AsyncExportRaw(grpc::ClientContext *context,
                      const proto::collector::trace::v1::ExportTraceServiceRequest &request,
                      grpc::CompletionQueue *cq) override
  {
    return NULL;
  }

  grpc::ClientAsyncResponseReaderInterface<proto::collector::trace::v1::ExportTraceServiceResponse>
      *PrepareAsyncExportRaw(grpc::ClientContext *context,
                             const proto::collector::trace::v1::ExportTraceServiceRequest &request,
                             grpc::CompletionQueue *cq) override
  {
    return NULL;
  }
};

// Benchmark Export() with span fields filled
void BM_OtlpExporterFullSpans(benchmark::State &state)
{
  std::unique_ptr<OtlpExporterTestPeer> testpeer(new OtlpExporterTestPeer());

  auto mock_stub = new FakeServiceStub();
  std::unique_ptr<proto::collector::trace::v1::TraceService::StubInterface> stub_interface(
      mock_stub);

  auto exporter = testpeer->GetExporter(stub_interface);

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
BENCHMARK(BM_OtlpExporterFullSpans);

// Benchmark Export() with empty spans
void BM_OtlpExporterEmptySpans(benchmark::State &state)
{
  std::unique_ptr<OtlpExporterTestPeer> testpeer(new OtlpExporterTestPeer());

  auto mock_stub = new FakeServiceStub();
  std::unique_ptr<proto::collector::trace::v1::TraceService::StubInterface> stub_interface(
      mock_stub);

  auto exporter = testpeer->GetExporter(stub_interface);

  while (state.KeepRunning())
  {
    std::unique_ptr<sdk::trace::Recordable> batch_arr[kBatchSize];

    for (int i = 0; i < kBatchSize; i++)
    {
      auto recordable = exporter->MakeRecordable();
      batch_arr[i] = std::move(recordable);
    }
    nostd::span<std::unique_ptr<sdk::trace::Recordable>> batch(batch_arr, kBatchSize);

    exporter->Export(batch);
  }
}
BENCHMARK(BM_OtlpExporterEmptySpans);

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE

BENCHMARK_MAIN();
