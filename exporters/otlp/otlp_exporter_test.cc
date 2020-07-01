#include "otlp_exporter.h"
#include "opentelemetry/sdk/trace/simple_processor.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/trace/provider.h"

#include <gtest/gtest.h>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

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

TEST(OtlpExporter, ExportInternal)
{
  auto exporter = std::unique_ptr<sdk::trace::SpanExporter>(new OtlpExporter);
  auto rec = exporter->MakeRecordable();

  // Name
  nostd::string_view name = "TestSpan";
  rec->SetName(name);
  // IDs
  rec->SetIds(GenerateRandomTraceId(), GenerateRandomSpanId(), GenerateRandomSpanId());
  // Start time
  core::SystemTimestamp start_timestamp(std::chrono::system_clock::now());
  rec->SetStartTime(start_timestamp);
  // Duration
  std::chrono::nanoseconds duration(10);
  rec->SetDuration(duration);

  nostd::span<std::unique_ptr<sdk::trace::Recordable>> recs(&rec, 1);
  auto result = exporter->Export(recs);

  EXPECT_EQ(sdk::trace::ExportResult::kSuccess, result);
}

TEST(OtlpExporter, ExportExternal)
{
  auto exporter  = std::unique_ptr<sdk::trace::SpanExporter>(new OtlpExporter);
  auto processor = std::shared_ptr<sdk::trace::SpanProcessor>(
    new sdk::trace::SimpleSpanProcessor(std::move(exporter)));
  auto provider = nostd::shared_ptr<trace::TracerProvider>(new sdk::trace::TracerProvider(processor));
  trace::Provider::SetTracerProvider(provider);

  nostd::shared_ptr<trace::Tracer> tracer = provider->GetTracer("test");

  auto span = tracer->StartSpan("TestSpan");
  span->End();
}
}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE