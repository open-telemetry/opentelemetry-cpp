#pragma once

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
namespace trace_api = opentelemetry::trace;

// TODO: Remove this placeholder with real class
class SpanContext
{
public:
explicit SpanContext(bool sampled_flag, bool is_remote)
  : traceFlags(sampled_flag), is_remote(is_remote) {}

uint8_t traceFlags;
bool is_remote;

const uint8_t FLAG_SAMPLED = 1; // 00000001

bool sampled = (traceFlags & FLAG_SAMPLED) == FLAG_SAMPLED;
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE