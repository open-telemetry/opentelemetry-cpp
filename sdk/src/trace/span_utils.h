#include "opentelemetry/version.h"
#include "src/common/random.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
namespace trace_api = opentelemetry::trace;

// Helper function to generate random trace id.
trace_api::TraceId GenerateRandomTraceId()
{
  uint8_t trace_id_buf[trace_api::TraceId::kSize];
  sdk::common::Random::GenerateRandomBuffer(trace_id_buf);
  return trace_api::TraceId(trace_id_buf);
}

// Helper function to generate random span id.
trace_api::SpanId GenerateRandomSpanId()
{
  uint8_t span_id_buf[trace_api::SpanId::kSize];
  sdk::common::Random::GenerateRandomBuffer(span_id_buf);
  return trace_api::SpanId(span_id_buf);
}
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
