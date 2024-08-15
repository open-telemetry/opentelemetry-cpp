// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/otlp/otlp_file_exporter_factory.h"
#include "opentelemetry/exporters/otlp/otlp_file_exporter_options.h"

#include "opentelemetry/exporters/otlp/otlp_file_exporter.h"

#include "opentelemetry/exporters/otlp/protobuf_include_prefix.h"

#include "google/protobuf/message_lite.h"
#include "opentelemetry/proto/collector/trace/v1/trace_service.pb.h"

#include "opentelemetry/exporters/otlp/protobuf_include_suffix.h"

#include "opentelemetry/sdk/trace/batch_span_processor.h"
#include "opentelemetry/sdk/trace/batch_span_processor_options.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"

#include "opentelemetry/trace/provider.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "nlohmann/json.hpp"

#include <chrono>
#include <iostream>
#include <sstream>

using namespace testing;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

namespace trace_api = opentelemetry::trace;
namespace resource  = opentelemetry::sdk::resource;

class ProtobufGlobalSymbolGuard
{
public:
  ProtobufGlobalSymbolGuard() {}
  ~ProtobufGlobalSymbolGuard() { google::protobuf::ShutdownProtobufLibrary(); }
};

template <class T, size_t N>
static nostd::span<T, N> MakeSpan(T (&array)[N])
{
  return nostd::span<T, N>(array);
}

class OtlpFileExporterTestPeer : public ::testing::Test
{
public:
  void ExportJsonIntegrationTest()
  {
    static ProtobufGlobalSymbolGuard global_symbol_guard;

    std::stringstream output;
    OtlpFileExporterOptions opts;
    opts.backend_options = std::ref(output);

    auto exporter = OtlpFileExporterFactory::Create(opts);

    resource::ResourceAttributes resource_attributes = {{"service.name", "unit_test_service"},
                                                        {"tenant.id", "test_user"}};
    resource_attributes["bool_value"]                = true;
    resource_attributes["int32_value"]               = static_cast<int32_t>(1);
    resource_attributes["uint32_value"]              = static_cast<uint32_t>(2);
    resource_attributes["int64_value"]               = static_cast<int64_t>(0x1100000000LL);
    resource_attributes["uint64_value"]              = static_cast<uint64_t>(0x1200000000ULL);
    resource_attributes["double_value"]              = static_cast<double>(3.1);
    resource_attributes["vec_bool_value"]            = std::vector<bool>{true, false, true};
    resource_attributes["vec_int32_value"]           = std::vector<int32_t>{1, 2};
    resource_attributes["vec_uint32_value"]          = std::vector<uint32_t>{3, 4};
    resource_attributes["vec_int64_value"]           = std::vector<int64_t>{5, 6};
    resource_attributes["vec_uint64_value"]          = std::vector<uint64_t>{7, 8};
    resource_attributes["vec_double_value"]          = std::vector<double>{3.2, 3.3};
    resource_attributes["vec_string_value"]          = std::vector<std::string>{"vector", "string"};
    auto resource = resource::Resource::Create(resource_attributes);

    auto processor_opts                  = sdk::trace::BatchSpanProcessorOptions();
    processor_opts.max_export_batch_size = 5;
    processor_opts.max_queue_size        = 5;
    processor_opts.schedule_delay_millis = std::chrono::milliseconds(256);

    auto processor = std::unique_ptr<sdk::trace::SpanProcessor>(
        new sdk::trace::BatchSpanProcessor(std::move(exporter), processor_opts));
    auto provider = nostd::shared_ptr<sdk::trace::TracerProvider>(
        new sdk::trace::TracerProvider(std::move(processor), resource));

    std::string report_trace_id;

    char trace_id_hex[2 * trace_api::TraceId::kSize] = {0};
    auto tracer                                      = provider->GetTracer("test");
    auto parent_span                                 = tracer->StartSpan("Test parent span");

    trace_api::StartSpanOptions child_span_opts = {};
    child_span_opts.parent                      = parent_span->GetContext();

    auto child_span = tracer->StartSpan("Test child span", child_span_opts);

    nostd::get<trace_api::SpanContext>(child_span_opts.parent)
        .trace_id()
        .ToLowerBase16(MakeSpan(trace_id_hex));
    report_trace_id.assign(trace_id_hex, sizeof(trace_id_hex));

    child_span->End();
    parent_span->End();

    provider->ForceFlush();

    output.flush();
    output.sync();
    auto check_json_text = output.str();
    if (!check_json_text.empty())
    {
      auto check_json = nlohmann::json::parse(check_json_text, nullptr, false);
      if (!check_json.is_discarded())
      {
        auto resource_span     = *check_json["resourceSpans"].begin();
        auto scope_span        = *resource_span["scopeSpans"].begin();
        auto span              = *scope_span["spans"].begin();
        auto received_trace_id = span["traceId"].get<std::string>();
        EXPECT_EQ(received_trace_id, report_trace_id);
      }
      else
      {
        FAIL() << "Failed to parse json:" << check_json_text;
      }
    }
  }
};

TEST(OtlpFileExporterTest, Shutdown)
{
  auto exporter = std::unique_ptr<opentelemetry::sdk::trace::SpanExporter>(new OtlpFileExporter());
  ASSERT_TRUE(exporter->Shutdown());

  nostd::span<std::unique_ptr<opentelemetry::sdk::trace::Recordable>> spans = {};

  auto result = exporter->Export(spans);
  EXPECT_EQ(result, opentelemetry::sdk::common::ExportResult::kFailure);
}

// Create spans, let processor call Export()
TEST_F(OtlpFileExporterTestPeer, ExportJsonIntegrationTestSync)
{
  ExportJsonIntegrationTest();
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
