// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <unordered_map>

#include "opentelemetry/exporters/otlp/otlp_grpc_exporter.h"
#include "opentelemetry/exporters/otlp/otlp_grpc_log_record_exporter.h"

#include "opentelemetry/exporters/otlp/protobuf_include_prefix.h"

#include "opentelemetry/proto/collector/logs/v1/logs_service_mock.grpc.pb.h"
#include "opentelemetry/proto/collector/trace/v1/trace_service_mock.grpc.pb.h"

#include "opentelemetry/exporters/otlp/protobuf_include_suffix.h"

#include "opentelemetry/logs/provider.h"
#include "opentelemetry/sdk/logs/batch_log_record_processor.h"
#include "opentelemetry/sdk/logs/exporter.h"
#include "opentelemetry/sdk/logs/logger_provider.h"
#include "opentelemetry/sdk/logs/recordable.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/processor.h"
#include "opentelemetry/sdk/trace/simple_processor_factory.h"
#include "opentelemetry/sdk/trace/tracer_provider_factory.h"
#include "opentelemetry/trace/provider.h"

#include <gtest/gtest.h>

#if defined(_MSC_VER)
#  include "opentelemetry/sdk/common/env_variables.h"
using opentelemetry::sdk::common::setenv;
using opentelemetry::sdk::common::unsetenv;
#endif

using namespace testing;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

class OtlpGrpcLogRecordExporterTestPeer : public ::testing::Test
{
public:
  std::unique_ptr<sdk::logs::LogRecordExporter> GetExporter(
      std::unique_ptr<proto::collector::logs::v1::LogsService::StubInterface> &stub_interface)
  {
    return std::unique_ptr<sdk::logs::LogRecordExporter>(
        new OtlpGrpcLogRecordExporter(std::move(stub_interface)));
  }

  std::unique_ptr<sdk::trace::SpanExporter> GetExporter(
      std::unique_ptr<proto::collector::trace::v1::TraceService::StubInterface> &stub_interface)
  {
    return std::unique_ptr<sdk::trace::SpanExporter>(
        new OtlpGrpcExporter(std::move(stub_interface)));
  }

  // Get the options associated with the given exporter.
  const OtlpGrpcLogRecordExporterOptions &GetOptions(
      std::unique_ptr<OtlpGrpcLogRecordExporter> &exporter)
  {
    return exporter->options_;
  }
};

TEST_F(OtlpGrpcLogRecordExporterTestPeer, ShutdownTest)
{
  auto mock_stub = new proto::collector::logs::v1::MockLogsServiceStub();
  std::unique_ptr<proto::collector::logs::v1::LogsService::StubInterface> stub_interface(mock_stub);
  auto exporter = GetExporter(stub_interface);

  auto recordable_1 = exporter->MakeRecordable();
  auto recordable_2 = exporter->MakeRecordable();

  // exporter shuold not be shutdown by default
  nostd::span<std::unique_ptr<sdk::logs::Recordable>> batch_1(&recordable_1, 1);
  EXPECT_CALL(*mock_stub, Export(_, _, _))
      .Times(Exactly(1))
      .WillOnce(Return(grpc::Status::OK))
      .WillOnce(Return(grpc::Status::CANCELLED));

  auto result = exporter->Export(batch_1);
  EXPECT_EQ(sdk::common::ExportResult::kSuccess, result);

  exporter->Shutdown();

  nostd::span<std::unique_ptr<sdk::logs::Recordable>> batch_2(&recordable_2, 1);
  result = exporter->Export(batch_2);
  EXPECT_EQ(sdk::common::ExportResult::kFailure, result);
}

// Call Export() directly
TEST_F(OtlpGrpcLogRecordExporterTestPeer, ExportUnitTest)
{
  auto mock_stub = new proto::collector::logs::v1::MockLogsServiceStub();
  std::unique_ptr<proto::collector::logs::v1::LogsService::StubInterface> stub_interface(mock_stub);
  auto exporter = GetExporter(stub_interface);

  auto recordable_1 = exporter->MakeRecordable();
  auto recordable_2 = exporter->MakeRecordable();

  // Test successful RPC
  nostd::span<std::unique_ptr<sdk::logs::Recordable>> batch_1(&recordable_1, 1);
  EXPECT_CALL(*mock_stub, Export(_, _, _)).Times(Exactly(1)).WillOnce(Return(grpc::Status::OK));
  auto result = exporter->Export(batch_1);
  EXPECT_EQ(sdk::common::ExportResult::kSuccess, result);

  // Test failed RPC
  nostd::span<std::unique_ptr<sdk::logs::Recordable>> batch_2(&recordable_2, 1);
  EXPECT_CALL(*mock_stub, Export(_, _, _))
      .Times(Exactly(1))
      .WillOnce(Return(grpc::Status::CANCELLED));
  result = exporter->Export(batch_2);
  EXPECT_EQ(sdk::common::ExportResult::kFailure, result);
}

// Create spans, let processor call Export()
TEST_F(OtlpGrpcLogRecordExporterTestPeer, ExportIntegrationTest)
{
  auto mock_stub = new proto::collector::logs::v1::MockLogsServiceStub();
  std::unique_ptr<proto::collector::logs::v1::LogsService::StubInterface> stub_interface(mock_stub);

  auto exporter = GetExporter(stub_interface);

  bool attribute_storage_bool_value[]                                = {true, false, true};
  int32_t attribute_storage_int32_value[]                            = {1, 2};
  uint32_t attribute_storage_uint32_value[]                          = {3, 4};
  int64_t attribute_storage_int64_value[]                            = {5, 6};
  uint64_t attribute_storage_uint64_value[]                          = {7, 8};
  double attribute_storage_double_value[]                            = {3.2, 3.3};
  opentelemetry::nostd::string_view attribute_storage_string_value[] = {"vector", "string"};

  auto provider = nostd::shared_ptr<sdk::logs::LoggerProvider>(new sdk::logs::LoggerProvider());
  provider->AddProcessor(
      std::unique_ptr<sdk::logs::LogRecordProcessor>(new sdk::logs::BatchLogRecordProcessor(
          std::move(exporter), 5, std::chrono::milliseconds(256), 1)));

  EXPECT_CALL(*mock_stub, Export(_, _, _))
      .Times(AtLeast(1))
      .WillRepeatedly(Return(grpc::Status::OK));

  uint8_t trace_id_bin[opentelemetry::trace::TraceId::kSize] = {
      '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
  opentelemetry::trace::TraceId trace_id{trace_id_bin};
  uint8_t span_id_bin[opentelemetry::trace::SpanId::kSize] = {'7', '6', '5', '4',
                                                              '3', '2', '1', '0'};
  opentelemetry::trace::SpanId span_id{span_id_bin};

  auto trace_mock_stub = new proto::collector::trace::v1::MockTraceServiceStub();
  std::unique_ptr<proto::collector::trace::v1::TraceService::StubInterface> trace_stub_interface(
      trace_mock_stub);

  auto trace_provider = opentelemetry::nostd::shared_ptr<opentelemetry::trace::TracerProvider>(
      opentelemetry::sdk::trace::TracerProviderFactory::Create(
          opentelemetry::sdk::trace::SimpleSpanProcessorFactory::Create(
              GetExporter(trace_stub_interface))));

  // Trace and Logs should both receive datas when links static gRPC on ELF ABI.
  EXPECT_CALL(*trace_mock_stub, Export(_, _, _))
      .Times(AtLeast(1))
      .WillRepeatedly(Return(grpc::Status::OK));

  {
    const std::string schema_url{"https://opentelemetry.io/schemas/1.11.0"};

    auto tracer = trace_provider->GetTracer("opentelelemtry_library", "", schema_url);
    opentelemetry::trace::Provider::SetTracerProvider(std::move(trace_provider));
    auto trace_span = tracer->StartSpan("test_log");
    opentelemetry::trace::Scope trace_scope{trace_span};

    auto logger = provider->GetLogger("test", "opentelelemtry_library", "", schema_url,
                                      {{"scope_key1", "scope_value"}, {"scope_key2", 2}});
    std::unordered_map<std::string, opentelemetry::common::AttributeValue> attributes;
    attributes["service.name"]     = "unit_test_service";
    attributes["tenant.id"]        = "test_user";
    attributes["bool_value"]       = true;
    attributes["int32_value"]      = static_cast<int32_t>(1);
    attributes["uint32_value"]     = static_cast<uint32_t>(2);
    attributes["int64_value"]      = static_cast<int64_t>(0x1100000000LL);
    attributes["uint64_value"]     = static_cast<uint64_t>(0x1200000000ULL);
    attributes["double_value"]     = static_cast<double>(3.1);
    attributes["vec_bool_value"]   = attribute_storage_bool_value;
    attributes["vec_int32_value"]  = attribute_storage_int32_value;
    attributes["vec_uint32_value"] = attribute_storage_uint32_value;
    attributes["vec_int64_value"]  = attribute_storage_int64_value;
    attributes["vec_uint64_value"] = attribute_storage_uint64_value;
    attributes["vec_double_value"] = attribute_storage_double_value;
    attributes["vec_string_value"] = attribute_storage_string_value;
    logger->EmitLogRecord(opentelemetry::logs::Severity::kInfo, "Log message", attributes,
                          trace_span->GetContext(), std::chrono::system_clock::now());
  }

  opentelemetry::trace::Provider::SetTracerProvider(
      opentelemetry::nostd::shared_ptr<opentelemetry::trace::TracerProvider>(
          new opentelemetry::trace::NoopTracerProvider()));
  trace_provider = opentelemetry::nostd::shared_ptr<opentelemetry::trace::TracerProvider>();
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
