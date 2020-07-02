#include "otlp_exporter.h"
#include "opentelemetry/sdk/trace/simple_processor.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/trace/provider.h"
#include "opentelemetry/proto/collector/trace/v1/trace_service_mock.grpc.pb.h"

#include <gtest/gtest.h>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

using namespace testing;

class OtlpExporterTestPeer : public ::testing::Test
{
public:
  std::unique_ptr<OtlpExporter> GetExporter(
    proto::collector::trace::v1::TraceService::StubInterface* mock_stub)
  {
      return std::unique_ptr<OtlpExporter>(
        new OtlpExporter(std::unique_ptr<proto::collector::trace::v1::TraceService::StubInterface>(mock_stub)));
  }

  trace::TraceId GenerateRandomTraceId()
  {
      uint8_t trace_id_buf[trace::TraceId::kSize];
      opentelemetry::sdk::common::Random::GenerateRandomBuffer(trace_id_buf);
      return trace::TraceId(trace_id_buf);
  }

  trace::SpanId GenerateRandomSpanId()
  {
      uint8_t span_id_buf[trace::SpanId::kSize];
      opentelemetry::sdk::common::Random::GenerateRandomBuffer(span_id_buf);
      return trace::SpanId(span_id_buf);
  }
};

TEST_F(OtlpExporterTestPeer, ExportInternal)
{
  auto mock_stub = new proto::collector::trace::v1::MockTraceServiceStub();
  EXPECT_CALL(*mock_stub, Export(_,_,_)).Times(AtLeast(1)).WillRepeatedly(Return(grpc::Status::OK));

  auto exporter = std::unique_ptr<sdk::trace::SpanExporter>(GetExporter(mock_stub));
  auto rec = exporter->MakeRecordable();

  // Set up recordable
  nostd::string_view name = "Test Span";
  rec->SetName(name);
  rec->SetIds(GenerateRandomTraceId(), GenerateRandomSpanId(), GenerateRandomSpanId());

  core::SystemTimestamp start_timestamp(std::chrono::system_clock::now());
  rec->SetStartTime(start_timestamp);

  std::chrono::nanoseconds duration(10);
  rec->SetDuration(duration);

  nostd::span<std::unique_ptr<sdk::trace::Recordable>> recs(&rec, 1);
  auto result = exporter->Export(recs);

  EXPECT_EQ(sdk::trace::ExportResult::kSuccess, result);
}

TEST_F(OtlpExporterTestPeer, ExportExternal)
{
  auto mock_stub = new proto::collector::trace::v1::MockTraceServiceStub();
  EXPECT_CALL(*mock_stub, Export(_,_,_)).Times(AtLeast(1)).WillRepeatedly(Return(grpc::Status::OK));

  auto exporter = std::unique_ptr<sdk::trace::SpanExporter>(GetExporter(mock_stub));

  auto processor = std::shared_ptr<sdk::trace::SpanProcessor>(
    new sdk::trace::SimpleSpanProcessor(std::move(exporter)));
  auto provider = nostd::shared_ptr<trace::TracerProvider>(new sdk::trace::TracerProvider(processor));
  trace::Provider::SetTracerProvider(provider);

  nostd::shared_ptr<trace::Tracer> tracer = provider->GetTracer("test");

  auto span = tracer->StartSpan("Test Span");
  span->End();
}
}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE