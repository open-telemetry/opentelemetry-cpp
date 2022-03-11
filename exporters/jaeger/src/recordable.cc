// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/jaeger/recordable.h"
#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/resource/experimental_semantic_conventions.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace jaeger
{

using namespace opentelemetry::sdk::resource;
namespace trace_api = opentelemetry::trace;

JaegerRecordable::JaegerRecordable() : span_{new thrift::Span} {}

void JaegerRecordable::PopulateAttribute(nostd::string_view key,
                                         const common::AttributeValue &value,
                                         std::vector<thrift::Tag> &tags)
{
  if (nostd::holds_alternative<int32_t>(value))
  {
    AddTag(std::string{key}, int64_t{nostd::get<int32_t>(value)}, tags);
  }
  else if (nostd::holds_alternative<uint32_t>(value))
  {
    AddTag(std::string{key}, int64_t{nostd::get<uint32_t>(value)}, tags);
  }
  else if (nostd::holds_alternative<int64_t>(value))
  {
    AddTag(std::string{key}, nostd::get<int64_t>(value), tags);
  }
  else if (nostd::holds_alternative<bool>(value))
  {
    AddTag(std::string{key}, nostd::get<bool>(value), tags);
  }
  else if (nostd::holds_alternative<double>(value))
  {
    AddTag(std::string{key}, nostd::get<double>(value), tags);
  }
  else if (nostd::holds_alternative<const char *>(value))
  {
    AddTag(std::string{key}, std::string{nostd::get<const char *>(value)}, tags);
  }
  else if (nostd::holds_alternative<nostd::string_view>(value))
  {
    AddTag(std::string{key}, std::string{nostd::get<nostd::string_view>(value)}, tags);
  }
  else if (nostd::holds_alternative<nostd::span<const bool>>(value))
  {
    for (const auto &val : nostd::get<nostd::span<const bool>>(value))
    {
      AddTag(std::string{key}, val, tags);
    }
  }
  else if (nostd::holds_alternative<nostd::span<const int32_t>>(value))
  {
    for (const auto &val : nostd::get<nostd::span<const int32_t>>(value))
    {
      AddTag(std::string{key}, int64_t{val}, tags);
    }
  }
  else if (nostd::holds_alternative<nostd::span<const int64_t>>(value))
  {
    for (const auto &val : nostd::get<nostd::span<const int64_t>>(value))
    {
      AddTag(std::string{key}, val, tags);
    }
  }
  else if (nostd::holds_alternative<nostd::span<const uint32_t>>(value))
  {
    for (const auto &val : nostd::get<nostd::span<const uint32_t>>(value))
    {
      AddTag(std::string{key}, int64_t{val}, tags);
    }
  }
  else if (nostd::holds_alternative<nostd::span<const double>>(value))
  {
    for (const auto &val : nostd::get<nostd::span<const double>>(value))
    {
      AddTag(std::string{key}, val, tags);
    }
  }
  else if (nostd::holds_alternative<nostd::span<const nostd::string_view>>(value))
  {
    for (const auto &val : nostd::get<nostd::span<const nostd::string_view>>(value))
    {
      AddTag(std::string{key}, std::string{val}, tags);
    }
  }
  else
  {
    OTEL_INTERNAL_LOG_ERROR(
        "[TRACE JAEGER Exporter] SetAttribute() failed, attribute type not supported ");
  }
}

void JaegerRecordable::PopulateAttribute(nostd::string_view key,
                                         const sdk::common::OwnedAttributeValue &value,
                                         std::vector<thrift::Tag> &tags)
{
  if (nostd::holds_alternative<int32_t>(value))
  {
    AddTag(std::string{key}, int64_t{nostd::get<int32_t>(value)}, tags);
  }
  else if (nostd::holds_alternative<uint32_t>(value))
  {
    AddTag(std::string{key}, int64_t{nostd::get<uint32_t>(value)}, tags);
  }
  else if (nostd::holds_alternative<int64_t>(value))
  {
    AddTag(std::string{key}, nostd::get<int64_t>(value), tags);
  }
  else if (nostd::holds_alternative<bool>(value))
  {
    AddTag(std::string{key}, nostd::get<bool>(value), tags);
  }
  else if (nostd::holds_alternative<double>(value))
  {
    AddTag(std::string{key}, nostd::get<double>(value), tags);
  }
  else if (nostd::holds_alternative<std::string>(value))
  {
    AddTag(std::string{key}, std::string{nostd::get<std::string>(value)}, tags);
  }
  else
  {
    OTEL_INTERNAL_LOG_ERROR(
        "[TRACE JAEGER Exporter] SetAttribute() failed, attribute type not supported ");
  }
}

void JaegerRecordable::SetIdentity(const trace::SpanContext &span_context,
                                   trace::SpanId parent_span_id) noexcept
{
  // IDs should be converted to big endian before transmission.
  // https://github.com/open-telemetry/opentelemetry-specification/blob/main/specification/trace/sdk_exporters/jaeger.md#ids
#if JAEGER_IS_LITTLE_ENDIAN == 1
  span_->__set_traceIdHigh(
      otel_bswap_64(*(reinterpret_cast<const int64_t *>(span_context.trace_id().Id().data()))));
  span_->__set_traceIdLow(
      otel_bswap_64(*(reinterpret_cast<const int64_t *>(span_context.trace_id().Id().data()) + 1)));
  span_->__set_spanId(
      otel_bswap_64(*(reinterpret_cast<const int64_t *>(span_context.span_id().Id().data()))));
  span_->__set_parentSpanId(
      otel_bswap_64(*(reinterpret_cast<const int64_t *>(parent_span_id.Id().data()))));
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

void JaegerRecordable::SetAttribute(nostd::string_view key,
                                    const common::AttributeValue &value) noexcept
{
  PopulateAttribute(key, value, tags_);
}

void JaegerRecordable::AddEvent(nostd::string_view name,
                                common::SystemTimestamp timestamp,
                                const common::KeyValueIterable &attributes) noexcept
{
  std::vector<thrift::Tag> tags;
  PopulateAttribute("event", static_cast<common::AttributeValue>(name.data()), tags);

  attributes.ForEachKeyValue([&](nostd::string_view key, common::AttributeValue value) noexcept {
    PopulateAttribute(key, value, tags);
    return true;
  });
  thrift::Log log;
  log.__set_fields(tags);
  log.__set_timestamp(
      std::chrono::duration_cast<std::chrono::microseconds>(timestamp.time_since_epoch()).count());
  logs_.push_back(log);
}

void JaegerRecordable::SetInstrumentationLibrary(
    const opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary
        &instrumentation_library) noexcept
{
  AddTag("otel.library.name", instrumentation_library.GetName(), tags_);
  AddTag("otel.library.version", instrumentation_library.GetVersion(), tags_);
}

void JaegerRecordable::AddLink(const trace::SpanContext &span_context,
                               const common::KeyValueIterable &attributes) noexcept
{
  // Note: "The Link’s attributes cannot be represented in Jaeger explicitly."
  // -- https://opentelemetry.io/docs/reference/specification/trace/sdk_exporters/jaeger/#links
  //
  // This implementation does not (currently) implement the optional conversion to span logs.

  thrift::SpanRef reference;

  reference.__set_refType(thrift::SpanRefType::FOLLOWS_FROM);

  // IDs should be converted to big endian before transmission.
  // https://github.com/open-telemetry/opentelemetry-specification/blob/main/specification/trace/sdk_exporters/jaeger.md#ids
#if JAEGER_IS_LITTLE_ENDIAN == 1
  reference.__set_traceIdHigh(
      otel_bswap_64(*(reinterpret_cast<const int64_t *>(span_context.trace_id().Id().data()))));
  reference.__set_traceIdLow(
      otel_bswap_64(*(reinterpret_cast<const int64_t *>(span_context.trace_id().Id().data()) + 1)));
  reference.__set_spanId(
      otel_bswap_64(*(reinterpret_cast<const int64_t *>(span_context.span_id().Id().data()))));
#else
  reference.__set_traceIdLow(
      *(reinterpret_cast<const int64_t *>(span_context.trace_id().Id().data())));
  reference.__set_traceIdHigh(
      *(reinterpret_cast<const int64_t *>(span_context.trace_id().Id().data()) + 1));
  reference.__set_spanId(*(reinterpret_cast<const int64_t *>(span_context.span_id().Id().data())));
#endif

  references_.push_back(reference);
}

void JaegerRecordable::SetStatus(trace::StatusCode code, nostd::string_view description) noexcept
{
  if (code == trace::StatusCode::kUnset)
  {
    return;
  }

  if (code == trace::StatusCode::kOk)
  {
    AddTag("otel.status_code", "OK", tags_);
  }
  else if (code == trace::StatusCode::kError)
  {
    AddTag("otel.status_code", "ERROR", tags_);
    AddTag("error", true, tags_);
  }

  AddTag("otel.status_description", std::string{description}, tags_);
}

void JaegerRecordable::SetName(nostd::string_view name) noexcept
{
  span_->__set_operationName(static_cast<std::string>(name));
}

void JaegerRecordable::SetResource(const opentelemetry::sdk::resource::Resource &resource) noexcept
{
  for (const auto &attribute_iter : resource.GetAttributes())
  {
    if (attribute_iter.first != "service.name")
    {
      PopulateAttribute(nostd::string_view{attribute_iter.first}, attribute_iter.second,
                        resource_tags_);
    }
    else
    {
      service_name_ = nostd::get<std::string>(attribute_iter.second);
    }
  }
}

void JaegerRecordable::SetStartTime(common::SystemTimestamp start_time) noexcept
{
  span_->__set_startTime(
      std::chrono::duration_cast<std::chrono::microseconds>(start_time.time_since_epoch()).count());
}

void JaegerRecordable::SetDuration(std::chrono::nanoseconds duration) noexcept
{
  span_->__set_duration(std::chrono::duration_cast<std::chrono::microseconds>(duration).count());
}

void JaegerRecordable::SetSpanKind(trace::SpanKind span_kind) noexcept
{
  const char *span_kind_str = nullptr;

  // map SpanKind to Jaeger tag span.kind.
  switch (span_kind)
  {
    case trace_api::SpanKind::kClient: {
      span_kind_str = "client";
      break;
    }
    case trace_api::SpanKind::kServer: {
      span_kind_str = "server";
      break;
    }
    case trace_api::SpanKind::kConsumer: {
      span_kind_str = "consumer";
      break;
    }
    case trace_api::SpanKind::kProducer: {
      span_kind_str = "producer";
      break;
    }
    default:
      break;
  }

  if (span_kind_str != nullptr)
  {
    AddTag("span.kind", span_kind_str, tags_);
  }
}

void JaegerRecordable::AddTag(const std::string &key,
                              const std::string &value,
                              std::vector<thrift::Tag> &tags)
{
  thrift::Tag tag;

  tag.__set_key(key);
  tag.__set_vType(thrift::TagType::STRING);
  tag.__set_vStr(value);

  tags.push_back(tag);
}

void JaegerRecordable::AddTag(const std::string &key,
                              const char *value,
                              std::vector<thrift::Tag> &tags)
{
  AddTag(key, std::string{value}, tags);
}

void JaegerRecordable::AddTag(const std::string &key, bool value, std::vector<thrift::Tag> &tags)
{
  thrift::Tag tag;

  tag.__set_key(key);
  tag.__set_vType(thrift::TagType::BOOL);
  tag.__set_vBool(value);

  tags.push_back(tag);
}

void JaegerRecordable::AddTag(const std::string &key, int64_t value, std::vector<thrift::Tag> &tags)
{
  thrift::Tag tag;

  tag.__set_key(key);
  tag.__set_vType(thrift::TagType::LONG);
  tag.__set_vLong(value);

  tags.push_back(tag);
}

void JaegerRecordable::AddTag(const std::string &key, double value, std::vector<thrift::Tag> &tags)
{
  thrift::Tag tag;

  tag.__set_key(key);
  tag.__set_vType(thrift::TagType::DOUBLE);
  tag.__set_vDouble(value);

  tags.push_back(tag);
}

}  // namespace jaeger
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
