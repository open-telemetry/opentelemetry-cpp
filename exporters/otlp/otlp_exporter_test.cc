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
TEST(OtlpExporter, ExportInternal)
{
  auto exporter = std::unique_ptr<sdk::trace::SpanExporter>(new OtlpExporter);

  auto rec = exporter->MakeRecordable();
  // Set span fields
  nostd::string_view name = "TestSpan";
  rec->SetName(name);

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