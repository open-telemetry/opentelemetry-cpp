#include "otlp_exporter.h"

#include <benchmark/benchmark.h>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

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

void BM_OtlpExporter(benchmark::State &state)
{
  std::unique_ptr<OtlpExporterTestPeer> testpeer(new OtlpExporterTestPeer());

  auto mock_stub = new FakeServiceStub();
  std::unique_ptr<proto::collector::trace::v1::TraceService::StubInterface> stub_interface(
      mock_stub);

  auto exporter = testpeer->GetExporter(stub_interface);

  while (state.KeepRunning())
  {
    const int kBatchSize = 100;
    std::unique_ptr<sdk::trace::Recordable> batch_arr[kBatchSize];

    for (int i = 0; i < kBatchSize; i++)
    {
      auto recordable = exporter->MakeRecordable();
      recordable->SetName("Hello");
      batch_arr[i] = std::unique_ptr<sdk::trace::Recordable>(recordable.release());
    }
    nostd::span<std::unique_ptr<sdk::trace::Recordable>> batch(batch_arr, kBatchSize);

    exporter->Export(batch);
  }
}
BENCHMARK(BM_OtlpExporter);

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE

BENCHMARK_MAIN();
