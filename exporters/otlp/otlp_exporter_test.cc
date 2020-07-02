#include "otlp_exporter.h"
#include "opentelemetry/sdk/trace/simple_processor.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/trace/provider.h"
#include "opentelemetry/proto/collector/trace/v1/trace_service_mock.grpc.pb.h"

#include <gtest/gtest.h>

using namespace testing;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

class OtlpExporterTestPeer : public ::testing::Test
{
public:
  std::unique_ptr<OtlpExporter> GetExporter(
    proto::collector::trace::v1::TraceService::StubInterface* mock_stub)
  {
    return std::unique_ptr<OtlpExporter>(
      new OtlpExporter(std::unique_ptr<proto::collector::trace::v1::TraceService::StubInterface>(mock_stub)));
  }
};

// Call Export() directly
TEST_F(OtlpExporterTestPeer, ExportUnitTest)
{
  auto mock_stub = new proto::collector::trace::v1::MockTraceServiceStub();
  auto exporter = std::shared_ptr<sdk::trace::SpanExporter>(GetExporter(mock_stub));

  auto recordable_1 = exporter->MakeRecordable();
  recordable_1->SetName("Test span 1");
  auto recordable_2 = exporter->MakeRecordable();
  recordable_2->SetName("Test span 2");

  // Test successful RPC
  nostd::span<std::unique_ptr<sdk::trace::Recordable>> batch_1(&recordable_1, 1);
  EXPECT_CALL(*mock_stub, Export(_,_,_)).Times(Exactly(1)).WillOnce(Return(grpc::Status::OK));
  auto result = exporter->Export(batch_1);
  EXPECT_EQ(sdk::trace::ExportResult::kSuccess, result);

  // Test failed RPC
  nostd::span<std::unique_ptr<sdk::trace::Recordable>> batch2(&recordable_2, 1);
  EXPECT_CALL(*mock_stub, Export(_,_,_)).Times(Exactly(1)).WillOnce(Return(grpc::Status::CANCELLED));
  auto result = exporter->Export(batch_2);
  EXPECT_EQ(sdk::trace::ExportResult::kFailure, result);
}

// Create spans, let processor call Export()
TEST_F(OtlpExporterTestPeer, ExportIntegrationTest)
{
  auto mock_stub = new proto::collector::trace::v1::MockTraceServiceStub();
  auto exporter = std::unique_ptr<sdk::trace::SpanExporter>(GetExporter(mock_stub));

  auto processor = std::shared_ptr<sdk::trace::SpanProcessor>(
    new sdk::trace::SimpleSpanProcessor(std::move(exporter)));
  auto provider = nostd::shared_ptr<trace::TracerProvider>(new sdk::trace::TracerProvider(processor));
  trace::Provider::SetTracerProvider(provider);

  nostd::shared_ptr<trace::Tracer> tracer = provider->GetTracer("test");

  EXPECT_CALL(*mock_stub, Export(_,_,_)).Times(AtLeast(1)).WillRepeatedly(Return(grpc::Status::OK));

  auto parent_span = tracer->StartSpan("Test parent span");
  auto child_span = tracer->StartSpan("Test child span");
  child_span->End();
  parent_span->End();
}
}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
