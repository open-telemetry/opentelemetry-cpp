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

Recordable::Recordable() : span_{new thrift::Span} {}

void Recordable::PopulateAttribute(nostd::string_view key, const common::AttributeValue &value)
{
  if (nostd::holds_alternative<int64_t>(value))
  {
    AddTag(std::string{key}, nostd::get<int64_t>(value));
  }
  else if (nostd::holds_alternative<bool>(value))
  {
    AddTag(std::string{key}, nostd::get<bool>(value));
  }
  else if (nostd::holds_alternative<double>(value))
  {
    AddTag(std::string{key}, nostd::get<double>(value));
  }
  else if (nostd::holds_alternative<nostd::string_view>(value))
  {
    AddTag(std::string{key}, std::string{nostd::get<nostd::string_view>(value)});
  }
  // TODO: extend other AttributeType to the types supported by Jaeger.
}

void Recordable::SetIdentity(const trace::SpanContext &span_context,
                             trace::SpanId parent_span_id) noexcept
{
  span_->__set_traceIdLow(
      *(reinterpret_cast<const int64_t *>(span_context.trace_id().Id().data())));
  span_->__set_traceIdHigh(
      *(reinterpret_cast<const int64_t *>(span_context.trace_id().Id().data()) + 1));
  span_->__set_spanId(*(reinterpret_cast<const int64_t *>(span_context.span_id().Id().data())));
  span_->__set_parentSpanId(*(reinterpret_cast<const int64_t *>(parent_span_id.Id().data())));

  // TODO: set trace_state.
}

void Recordable::SetAttribute(nostd::string_view key, const common::AttributeValue &value) noexcept
{
  PopulateAttribute(key, value);
}

void Recordable::AddEvent(nostd::string_view name,
                          core::SystemTimestamp timestamp,
                          const common::KeyValueIterable &attributes) noexcept
{
  // TODO: convert event to Jaeger Log
}

void Recordable::AddLink(const trace::SpanContext &span_context,
                         const common::KeyValueIterable &attributes) noexcept
{
  // TODO: convert link to SpanRefernece
}

void Recordable::SetStatus(trace::StatusCode code, nostd::string_view description) noexcept
{
  if (code == trace::StatusCode::kUnset)
  {
    return;
  }

  if (code == trace::StatusCode::kOk)
  {
    AddTag("otel.status_code", "OK");
  }
  else if (code == trace::StatusCode::kError)
  {
    AddTag("otel.status_code", "ERROR");
    AddTag("error", true);
  }

  AddTag("otel.status_description", std::string{description});
}

void Recordable::SetName(nostd::string_view name) noexcept
{
  span_->__set_operationName(static_cast<std::string>(name));
}

void Recordable::SetStartTime(core::SystemTimestamp start_time) noexcept
{
  span_->__set_startTime(
      std::chrono::duration_cast<std::chrono::microseconds>(start_time.time_since_epoch()).count());
}

void Recordable::SetDuration(std::chrono::nanoseconds duration) noexcept
{
  span_->__set_duration(std::chrono::duration_cast<std::chrono::microseconds>(duration).count());
}

void Recordable::SetSpanKind(trace::SpanKind span_kind) noexcept
{
  switch (span_kind)
  {
    case opentelemetry::trace::SpanKind::kClient: {
      AddTag("span.kind", "client");
      break;
    }
    case opentelemetry::trace::SpanKind::kServer: {
      AddTag("span.kind", "server");
      break;
    }
    case opentelemetry::trace::SpanKind::kConsumer: {
      AddTag("span.kind", "consumer");
      break;
    }
    case opentelemetry::trace::SpanKind::kProducer: {
      AddTag("span.kind", "producer");
      break;
    }
    default:
      break;
  }
}

void Recordable::AddTag(const std::string &key, const std::string &value)
{
  thrift::Tag tag;

  tag.__set_key(key);
  tag.__set_vType(thrift::TagType::STRING);
  tag.__set_vStr(value);

  tags_.push_back(tag);
}

void Recordable::AddTag(const std::string &key, const char *value)
{
  AddTag(key, std::string{value});
}

void Recordable::AddTag(const std::string &key, bool value)
{
  thrift::Tag tag;

  tag.__set_key(key);
  tag.__set_vType(thrift::TagType::BOOL);
  tag.__set_vBool(value);

  tags_.push_back(tag);
}

void Recordable::AddTag(const std::string &key, int64_t value)
{
  thrift::Tag tag;

  tag.__set_key(key);
  tag.__set_vType(thrift::TagType::LONG);
  tag.__set_vLong(value);

  tags_.push_back(tag);
}

void Recordable::AddTag(const std::string &key, double value)
{
  thrift::Tag tag;

  tag.__set_key(key);
  tag.__set_vType(thrift::TagType::DOUBLE);
  tag.__set_vDouble(value);

  tags_.push_back(tag);
}

}  // namespace jaeger
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
