// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

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
  else if (nostd::holds_alternative<const char *>(value))
  {
    AddTag(std::string{key}, std::string{nostd::get<const char *>(value)});
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
  // IDs should be converted to big endian before transmission.
  // https://github.com/open-telemetry/opentelemetry-specification/blob/main/specification/trace/sdk_exporters/jaeger.md#ids
#if JAEGER_IS_LITTLE_ENDIAN == 1
  span_->__set_traceIdHigh(
      bswap_64(*(reinterpret_cast<const int64_t *>(span_context.trace_id().Id().data()))));
  span_->__set_traceIdLow(
      bswap_64(*(reinterpret_cast<const int64_t *>(span_context.trace_id().Id().data()) + 1)));
  span_->__set_spanId(
      bswap_64(*(reinterpret_cast<const int64_t *>(span_context.span_id().Id().data()))));
  span_->__set_parentSpanId(
      bswap_64(*(reinterpret_cast<const int64_t *>(parent_span_id.Id().data()))));
#else
  span_->__set_traceIdLow(
      *(reinterpret_cast<const int64_t *>(span_context.trace_id().Id().data())));
  span_->__set_traceIdHigh(
      *(reinterpret_cast<const int64_t *>(span_context.trace_id().Id().data()) + 1));
  span_->__set_spanId(*(reinterpret_cast<const int64_t *>(span_context.span_id().Id().data())));
  span_->__set_parentSpanId(*(reinterpret_cast<const int64_t *>(parent_span_id.Id().data())));
#endif

  // TODO: set trace_state.
}

void Recordable::SetAttribute(nostd::string_view key, const common::AttributeValue &value) noexcept
{
  PopulateAttribute(key, value);
}

void Recordable::AddEvent(nostd::string_view name,
                          common::SystemTimestamp timestamp,
                          const common::KeyValueIterable &attributes) noexcept
{
  // TODO: convert event to Jaeger Log
}

void Recordable::SetInstrumentationLibrary(
    const opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary
        &instrumentation_library) noexcept
{}

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

void Recordable::SetResource(const opentelemetry::sdk::resource::Resource &resource) noexcept
{
  // only service.name attribute is supported by specs as of now.
  auto attributes = resource.GetAttributes();
  if (attributes.find("service.name") != attributes.end())
  {
    service_name_ = nostd::get<std::string>(attributes["service.name"]);
  }
}

void Recordable::SetStartTime(common::SystemTimestamp start_time) noexcept
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
  const char *span_kind_str = nullptr;

  // map SpanKind to Jaeger tag span.kind.
  switch (span_kind)
  {
    case opentelemetry::trace::SpanKind::kClient: {
      span_kind_str = "client";
      break;
    }
    case opentelemetry::trace::SpanKind::kServer: {
      span_kind_str = "server";
      break;
    }
    case opentelemetry::trace::SpanKind::kConsumer: {
      span_kind_str = "consumer";
      break;
    }
    case opentelemetry::trace::SpanKind::kProducer: {
      span_kind_str = "producer";
      break;
    }
    default:
      break;
  }

  if (span_kind_str != nullptr)
  {
    AddTag("span.kind", span_kind_str);
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
