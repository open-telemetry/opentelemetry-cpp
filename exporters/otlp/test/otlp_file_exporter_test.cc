// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <map>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "opentelemetry/common/key_value_iterable_view.h"
#include "opentelemetry/common/timestamp.h"
#include "opentelemetry/context/context.h"
#include "opentelemetry/exporters/otlp/otlp_file_exporter.h"
#include "opentelemetry/exporters/otlp/otlp_file_exporter_factory.h"
#include "opentelemetry/exporters/otlp/otlp_file_exporter_options.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/utility.h"  // IWYU pragma: keep
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/sdk/common/exporter_utils.h"
#include "opentelemetry/sdk/instrumentationscope/instrumentation_scope.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/sdk/trace/batch_span_processor.h"
#include "opentelemetry/sdk/trace/batch_span_processor_options.h"
#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/processor.h"
#include "opentelemetry/sdk/trace/recordable.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/trace/span.h"
#include "opentelemetry/trace/span_context.h"
#include "opentelemetry/trace/span_id.h"
#include "opentelemetry/trace/span_metadata.h"
#include "opentelemetry/trace/span_startoptions.h"
#include "opentelemetry/trace/trace_flags.h"
#include "opentelemetry/trace/trace_id.h"
#include "opentelemetry/trace/tracer.h"
#include "opentelemetry/version.h"

// clang-format off
#include "opentelemetry/exporters/otlp/protobuf_include_prefix.h" // IWYU pragma: keep
#include "google/protobuf/message_lite.h"
#include "opentelemetry/exporters/otlp/protobuf_include_suffix.h" // IWYU pragma: keep
// clang-format on

#if OPENTELEMETRY_ABI_VERSION_NO >= 2
#  include <unordered_map>

#  include "opentelemetry/common/attribute_value.h"
#  include "opentelemetry/nostd/utility.h"
#endif

using namespace testing;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

namespace trace_api = opentelemetry::trace;
namespace resource  = opentelemetry::sdk::resource;

namespace
{
class ProtobufGlobalSymbolGuard
{
public:
  ProtobufGlobalSymbolGuard() = default;
  ~ProtobufGlobalSymbolGuard() { google::protobuf::ShutdownProtobufLibrary(); }
  ProtobufGlobalSymbolGuard(const ProtobufGlobalSymbolGuard &)            = delete;
  ProtobufGlobalSymbolGuard &operator=(const ProtobufGlobalSymbolGuard &) = delete;
  ProtobufGlobalSymbolGuard(ProtobufGlobalSymbolGuard &&)                 = delete;
  ProtobufGlobalSymbolGuard &operator=(ProtobufGlobalSymbolGuard &&)      = delete;
};
}  // namespace

template <class T, size_t N>
static nostd::span<T, N> MakeSpan(T (&array)[N])
{
  return nostd::span<T, N>(array);
}

namespace
{
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
    auto resource = resource::Resource::Create(resource_attributes, "resource_url");

    auto processor_opts                  = sdk::trace::BatchSpanProcessorOptions();
    processor_opts.max_export_batch_size = 5;
    processor_opts.max_queue_size        = 5;
    processor_opts.schedule_delay_millis = std::chrono::milliseconds(256);

    std::unique_ptr<sdk::trace::SpanProcessor> processor =
        std::make_unique<sdk::trace::BatchSpanProcessor>(std::move(exporter), processor_opts);
    auto provider = nostd::shared_ptr<sdk::trace::TracerProvider>(
        new sdk::trace::TracerProvider(std::move(processor), resource));

    std::string report_trace_id;

#if OPENTELEMETRY_ABI_VERSION_NO >= 2
    std::unordered_map<std::string, common::AttributeValue> scope_attributes;
    scope_attributes["scope_key"] = common::AttributeValue("scope_value");
    auto tracer = provider->GetTracer("scope_name", "scope_version", "scope_url", scope_attributes);
#else
    auto tracer = provider->GetTracer("scope_name", "scope_version", "scope_url");
#endif

    auto parent_span = tracer->StartSpan("Test parent span");

    char trace_id_hex[2 * trace_api::TraceId::kSize] = {0};

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
      // If the exporting is splited to two standalone resource_span, just checking the first one.
      std::string::size_type eol = check_json_text.find('\n');
      if (eol != std::string::npos)
      {
        check_json_text = check_json_text.substr(0, eol);
      }
      auto check_json = nlohmann::json::parse(check_json_text, nullptr, false);
      if (!check_json.is_discarded())
      {
        auto resource_span = *check_json["resourceSpans"].begin();
        auto scope_span    = *resource_span["scopeSpans"].begin();
        auto scope         = scope_span["scope"];
        auto span          = *scope_span["spans"].begin();

#if OPENTELEMETRY_ABI_VERSION_NO >= 2
        ASSERT_EQ(1, scope["attributes"].size());
        const auto scope_attribute = scope["attributes"].front();
        EXPECT_EQ("scope_key", scope_attribute["key"].get<std::string>());
        EXPECT_EQ("scope_value", scope_attribute["value"]["stringValue"].get<std::string>());
#endif
        EXPECT_EQ("resource_url", resource_span["schemaUrl"].get<std::string>());
        EXPECT_EQ("scope_url", scope_span["schemaUrl"].get<std::string>());
        EXPECT_EQ("scope_name", scope["name"].get<std::string>());
        EXPECT_EQ("scope_version", scope["version"].get<std::string>());
        EXPECT_EQ(report_trace_id, span["traceId"].get<std::string>());
      }
      else
      {
        FAIL() << "Failed to parse json:" << check_json_text;
      }
    }
  }
};
}  // namespace

TEST(OtlpFileExporterTest, Shutdown)
{
  std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> exporter =
      std::make_unique<OtlpFileExporter>();
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

TEST(OtlpFileExporterTest, ExportJsonGoldenBody)
{
  static ProtobufGlobalSymbolGuard global_symbol_guard;

  std::stringstream output;
  OtlpFileExporterOptions opts;
  opts.backend_options = std::ref(output);
  auto exporter        = OtlpFileExporterFactory::Create(opts);

  auto recordable = exporter->MakeRecordable();

  const uint8_t trace_id_bytes[trace_api::TraceId::kSize] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
                                                             0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c,
                                                             0x0d, 0x0e, 0x0f, 0x10};
  const uint8_t span_id_bytes[trace_api::SpanId::kSize]   = {0x11, 0x12, 0x13, 0x14,
                                                             0x15, 0x16, 0x17, 0x18};
  const uint8_t parent_span_id_bytes[trace_api::SpanId::kSize] = {0x21, 0x22, 0x23, 0x24,
                                                                  0x25, 0x26, 0x27, 0x28};
  const uint8_t link_trace_id_bytes[trace_api::TraceId::kSize] = {
      0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38,
      0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 0x40};
  const uint8_t link_span_id_bytes[trace_api::SpanId::kSize] = {0x41, 0x42, 0x43, 0x44,
                                                                0x45, 0x46, 0x47, 0x48};

  trace_api::SpanContext span_context(
      trace_api::TraceId(trace_id_bytes), trace_api::SpanId(span_id_bytes),
      trace_api::TraceFlags(trace_api::TraceFlags::kIsSampled), false);
  recordable->SetIdentity(span_context, trace_api::SpanId(parent_span_id_bytes));

  recordable->SetName("golden span");
  recordable->SetSpanKind(trace_api::SpanKind::kServer);
  recordable->SetStartTime(
      opentelemetry::common::SystemTimestamp(std::chrono::nanoseconds(1000000000)));
  recordable->SetDuration(std::chrono::nanoseconds(500000000));
  recordable->SetStatus(trace_api::StatusCode::kOk, "all good");
  recordable->SetAttribute("bool_attr", true);
  recordable->SetAttribute("int64_attr", static_cast<int64_t>(-42));
  recordable->SetAttribute("uint64_attr", static_cast<uint64_t>(42));
  recordable->SetAttribute("double_attr", static_cast<double>(3.5));
  recordable->SetAttribute("string_attr", "golden value");

  std::map<std::string, std::string> event_attribute_map = {{"event_key", "event_value"}};
  recordable->AddEvent("golden event",
                       opentelemetry::common::SystemTimestamp(std::chrono::nanoseconds(1000500000)),
                       opentelemetry::common::MakeAttributes(event_attribute_map));

  trace_api::SpanContext link_context(
      trace_api::TraceId(link_trace_id_bytes), trace_api::SpanId(link_span_id_bytes),
      trace_api::TraceFlags(trace_api::TraceFlags::kIsSampled), false);
  std::map<std::string, std::string> link_attribute_map = {{"link_key", "link_value"}};
  recordable->AddLink(link_context, opentelemetry::common::MakeAttributes(link_attribute_map));

  auto instrumentation_scope =
      opentelemetry::sdk::instrumentationscope::InstrumentationScope::Create(
          "golden_scope", "golden_scope_version");
  recordable->SetInstrumentationScope(*instrumentation_scope);

  nostd::span<std::unique_ptr<opentelemetry::sdk::trace::Recordable>> batch(&recordable, 1);
  EXPECT_EQ(opentelemetry::sdk::common::ExportResult::kSuccess, exporter->Export(batch));

  output.flush();
  std::string captured_body = output.str();
  if (!captured_body.empty() && captured_body.back() == '\n')
  {
    captured_body.pop_back();
  }

  static constexpr char kExpectedBody[] =
      R"({"resourceSpans":[{"scopeSpans":[{"scope":{"name":"golden_scope","version":"golden_scope_version"},"spans":[{"attributes":[{"key":"bool_attr","value":{"boolValue":true}},{"key":"int64_attr","value":{"intValue":"-42"}},{"key":"uint64_attr","value":{"intValue":"42"}},{"key":"double_attr","value":{"doubleValue":3.5}},{"key":"string_attr","value":{"stringValue":"golden value"}}],"endTimeUnixNano":"1500000000","events":[{"attributes":[{"key":"event_key","value":{"stringValue":"event_value"}}],"name":"golden event","timeUnixNano":"1000500000"}],"kind":2,"links":[{"attributes":[{"key":"link_key","value":{"stringValue":"link_value"}}],"spanId":"4142434445464748","traceId":"3132333435363738393a3b3c3d3e3f40"}],"name":"golden span","parentSpanId":"2122232425262728","spanId":"1112131415161718","startTimeUnixNano":"1000000000","status":{"code":1},"traceId":"0102030405060708090a0b0c0d0e0f10"}]}]}]})";

  ASSERT_EQ(std::string(kExpectedBody), captured_body);
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
