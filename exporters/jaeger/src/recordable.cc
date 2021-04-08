// Copyright 2021, OpenTelemetry Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <opentelemetry/exporters/jaeger/recordable.h>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace jaeger
{

void PopulateAttribute(nostd::string_view key, const opentelemetry::common::AttributeValue &value)
{}

void Recordable::SetIds(trace::TraceId trace_id,
                        trace::SpanId span_id,
                        trace::SpanId parent_span_id) noexcept
{
  span_->__set_traceIdLow(*(reinterpret_cast<const int64_t *>(trace_id.Id().data())));
  span_->__set_traceIdHigh(*(reinterpret_cast<const int64_t *>(trace_id.Id().data()) + 1));
  span_->__set_spanId(*(reinterpret_cast<const int64_t *>(trace_id.Id().data())));
  span_->__set_parentSpanId(*(reinterpret_cast<const int64_t *>(parent_span_id.Id().data())));
}

void Recordable::SetAttribute(nostd::string_view key,
                              const opentelemetry::common::AttributeValue &value) noexcept
{}

void Recordable::AddEvent(nostd::string_view name,
                          core::SystemTimestamp timestamp,
                          const common::KeyValueIterable &attributes) noexcept
{}

void Recordable::AddLink(const opentelemetry::trace::SpanContext &span_context,
                         const common::KeyValueIterable &attributes) noexcept
{}

void Recordable::SetStatus(trace::StatusCode code, nostd::string_view description) noexcept {}

void Recordable::SetName(nostd::string_view name) noexcept
{
  span_->__set_operationName(static_cast<std::string>(name));
}

void Recordable::SetStartTime(opentelemetry::core::SystemTimestamp start_time) noexcept
{
  span_->__set_startTime(start_time.time_since_epoch().count());
}

void Recordable::SetDuration(std::chrono::nanoseconds duration) noexcept
{
  span_->__set_duration(std::chrono::duration_cast<std::chrono::microseconds>(duration).count());
}

void Recordable::SetSpanKind(opentelemetry::trace::SpanKind spand_kind) noexcept {}

}  // namespace jaeger
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
