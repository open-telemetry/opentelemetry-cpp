// Copyright 2021, OpenTelemetry Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "opentelemetry/sdk/trace/id_generator.h"
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
