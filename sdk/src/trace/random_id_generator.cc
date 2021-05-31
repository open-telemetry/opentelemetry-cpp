// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/trace/random_id_generator.h"
#include "opentelemetry/version.h"
#include "src/common/random.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{

opentelemetry::trace::SpanId RandomIdGenerator::GenerateSpanId() noexcept
{
  uint8_t span_id_buf[opentelemetry::trace::SpanId::kSize];
  sdk::common::Random::GenerateRandomBuffer(span_id_buf);
  return opentelemetry::trace::SpanId(span_id_buf);
}

opentelemetry::trace::TraceId RandomIdGenerator::GenerateTraceId() noexcept
{
  uint8_t trace_id_buf[opentelemetry::trace::TraceId::kSize];
  sdk::common::Random::GenerateRandomBuffer(trace_id_buf);
  return opentelemetry::trace::TraceId(trace_id_buf);
}
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
