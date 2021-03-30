#include "opentelemetry/exporters/otlp/otlp_exporter.h"
#include "opentelemetry/proto/collector/trace/v1/trace_service_mock.grpc.pb.h"
#include "opentelemetry/sdk/trace/simple_processor.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/trace/provider.h"

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
  std::unique_ptr<sdk::trace::SpanExporter> GetExporter(
      std::unique_ptr<proto::collector::trace::v1::TraceService::StubInterface> &stub_interface)
  {
    return std::unique_ptr<sdk::trace::SpanExporter>(new OtlpExporter(std::move(stub_interface)));
  }

  // Get the options associated with the given exporter.
  const OtlpExporterOptions &GetOptions(std::unique_ptr<OtlpExporter> &exporter)
  {
    return exporter->options_;
  }
};

// Call Export() directly
TEST_F(OtlpExporterTestPeer, ExportUnitTest)
{
  auto mock_stub = new proto::collector::trace::v1::MockTraceServiceStub();
  std::unique_ptr<proto::collector::trace::v1::TraceService::StubInterface> stub_interface(
      mock_stub);
  auto exporter = GetExporter(stub_interface);

  auto recordable_1 = exporter->MakeRecordable();
  recordable_1->SetName("Test span 1");
  auto recordable_2 = exporter->MakeRecordable();
  recordable_2->SetName("Test span 2");

  // Test successful RPC
  nostd::span<std::unique_ptr<sdk::trace::Recordable>> batch_1(&recordable_1, 1);
  EXPECT_CALL(*mock_stub, Export(_, _, _)).Times(Exactly(1)).WillOnce(Return(grpc::Status::OK));
  auto result = exporter->Export(batch_1);
  EXPECT_EQ(sdk::trace::ExportResult::kSuccess, result);

  // Test failed RPC
  nostd::span<std::unique_ptr<sdk::trace::Recordable>> batch_2(&recordable_2, 1);
  EXPECT_CALL(*mock_stub, Export(_, _, _))
      .Times(Exactly(1))
      .WillOnce(Return(grpc::Status::CANCELLED));
  result = exporter->Export(batch_2);
  EXPECT_EQ(sdk::trace::ExportResult::kFailure, result);
}

// Create spans, let processor call Export()
TEST_F(OtlpExporterTestPeer, ExportIntegrationTest)
{
  auto mock_stub = new proto::collector::trace::v1::MockTraceServiceStub();
  std::unique_ptr<proto::collector::trace::v1::TraceService::StubInterface> stub_interface(
      mock_stub);

  auto exporter = GetExporter(stub_interface);

  auto processor = std::shared_ptr<sdk::trace::SpanProcessor>(
      new sdk::trace::SimpleSpanProcessor(std::move(exporter)));
  auto provider =
      nostd::shared_ptr<trace::TracerProvider>(new sdk::trace::TracerProvider(processor));
  auto tracer = provider->GetTracer("test");

  EXPECT_CALL(*mock_stub, Export(_, _, _))
      .Times(AtLeast(1))
      .WillRepeatedly(Return(grpc::Status::OK));

  auto parent_span = tracer->StartSpan("Test parent span");
  auto child_span  = tracer->StartSpan("Test child span");
  child_span->End();
  parent_span->End();
}

// Test exporter configuration options
TEST_F(OtlpExporterTestPeer, ConfigTest)
{
  OtlpExporterOptions opts;
  opts.endpoint = "localhost:45454";
  std::unique_ptr<OtlpExporter> exporter(new OtlpExporter(opts));
  EXPECT_EQ(GetOptions(exporter).endpoint, "localhost:45454");
}

// Test exporter configuration options with use_ssl_credentials
TEST_F(OtlpExporterTestPeer, ConfigSslCredentialsTest)
{
  std::string cacert_str = "--begin and end fake cert--";
  OtlpExporterOptions opts;
  opts.use_ssl_credentials              = true;
  opts.ssl_credentials_cacert_as_string = cacert_str;
  std::unique_ptr<OtlpExporter> exporter(new OtlpExporter(opts));
  EXPECT_EQ(GetOptions(exporter).ssl_credentials_cacert_as_string, cacert_str);
  EXPECT_EQ(GetOptions(exporter).use_ssl_credentials, true);
}
}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
