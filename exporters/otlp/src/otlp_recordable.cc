#include "opentelemetry/exporters/otlp/otlp_recordable.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

//
// See `attribute_value.h` for details.
//
const int kAttributeValueSize      = 15;
const int kOwnedAttributeValueSize = 15;

void OtlpRecordable::SetIdentity(const opentelemetry::trace::SpanContext &span_context,
                                 opentelemetry::trace::SpanId parent_span_id) noexcept
{
  span_.set_trace_id(reinterpret_cast<const char *>(span_context.trace_id().Id().data()),
                     trace::TraceId::kSize);
  span_.set_span_id(reinterpret_cast<const char *>(span_context.span_id().Id().data()),
                    trace::SpanId::kSize);
  span_.set_parent_span_id(reinterpret_cast<const char *>(parent_span_id.Id().data()),
                           trace::SpanId::kSize);
  span_.set_trace_state(span_context.trace_state()->ToHeader());
}

void PopulateAttribute(opentelemetry::proto::common::v1::KeyValue *attribute,
                       nostd::string_view key,
                       const opentelemetry::common::AttributeValue &value)
{
  // Assert size of variant to ensure that this method gets updated if the variant
  // definition changes
  static_assert(
      nostd::variant_size<opentelemetry::common::AttributeValue>::value == kAttributeValueSize,
      "AttributeValue contains unknown type");

  attribute->set_key(key.data(), key.size());

  if (nostd::holds_alternative<bool>(value))
  {
    attribute->mutable_value()->set_bool_value(nostd::get<bool>(value));
  }
  else if (nostd::holds_alternative<int>(value))
  {
    attribute->mutable_value()->set_int_value(nostd::get<int>(value));
  }
  else if (nostd::holds_alternative<int64_t>(value))
  {
    attribute->mutable_value()->set_int_value(nostd::get<int64_t>(value));
  }
  else if (nostd::holds_alternative<unsigned int>(value))
  {
    attribute->mutable_value()->set_int_value(nostd::get<unsigned int>(value));
  }
  else if (nostd::holds_alternative<uint64_t>(value))
  {
    attribute->mutable_value()->set_int_value(nostd::get<uint64_t>(value));
  }
  else if (nostd::holds_alternative<double>(value))
  {
    attribute->mutable_value()->set_double_value(nostd::get<double>(value));
  }
  else if (nostd::holds_alternative<nostd::string_view>(value))
  {
    attribute->mutable_value()->set_string_value(nostd::get<nostd::string_view>(value).data(),
                                                 nostd::get<nostd::string_view>(value).size());
  }
  else if (nostd::holds_alternative<nostd::span<const uint8_t>>(value))
  {
    for (const auto &val : nostd::get<nostd::span<const uint8_t>>(value))
    {
      attribute->mutable_value()->mutable_array_value()->add_values()->set_int_value(val);
    }
  }
  else if (nostd::holds_alternative<nostd::span<const bool>>(value))
  {
    for (const auto &val : nostd::get<nostd::span<const bool>>(value))
    {
      attribute->mutable_value()->mutable_array_value()->add_values()->set_bool_value(val);
    }
  }
  else if (nostd::holds_alternative<nostd::span<const int>>(value))
  {
    for (const auto &val : nostd::get<nostd::span<const int>>(value))
    {
      attribute->mutable_value()->mutable_array_value()->add_values()->set_int_value(val);
    }
  }
  else if (nostd::holds_alternative<nostd::span<const int64_t>>(value))
  {
    for (const auto &val : nostd::get<nostd::span<const int64_t>>(value))
    {
      attribute->mutable_value()->mutable_array_value()->add_values()->set_int_value(val);
    }
  }
  else if (nostd::holds_alternative<nostd::span<const unsigned int>>(value))
  {
    for (const auto &val : nostd::get<nostd::span<const unsigned int>>(value))
    {
      attribute->mutable_value()->mutable_array_value()->add_values()->set_int_value(val);
    }
  }
  else if (nostd::holds_alternative<nostd::span<const uint64_t>>(value))
  {
    for (const auto &val : nostd::get<nostd::span<const uint64_t>>(value))
    {
      attribute->mutable_value()->mutable_array_value()->add_values()->set_int_value(val);
    }
  }
  else if (nostd::holds_alternative<nostd::span<const double>>(value))
  {
    for (const auto &val : nostd::get<nostd::span<const double>>(value))
    {
      attribute->mutable_value()->mutable_array_value()->add_values()->set_double_value(val);
    }
  }
  else if (nostd::holds_alternative<nostd::span<const nostd::string_view>>(value))
  {
    for (const auto &val : nostd::get<nostd::span<const nostd::string_view>>(value))
    {
      attribute->mutable_value()->mutable_array_value()->add_values()->set_string_value(val.data(),
                                                                                        val.size());
    }
  }
}

/** Maps from C++ attribute into OTLP proto attribute. */
void PopulateAttribute(opentelemetry::proto::common::v1::KeyValue *attribute,
                       nostd::string_view key,
                       const sdk::common::OwnedAttributeValue &value)
{
  // Assert size of variant to ensure that this method gets updated if the variant
  // definition changes
  static_assert(
      nostd::variant_size<opentelemetry::common::AttributeValue>::value == kOwnedAttributeValueSize,
      "AttributeValue contains unknown type");

  attribute->set_key(key.data(), key.size());

  if (nostd::holds_alternative<bool>(value))
  {
    attribute->mutable_value()->set_bool_value(nostd::get<bool>(value));
  }
  else if (nostd::holds_alternative<int32_t>(value))
  {
    attribute->mutable_value()->set_int_value(nostd::get<int32_t>(value));
  }
  else if (nostd::holds_alternative<int64_t>(value))
  {
    attribute->mutable_value()->set_int_value(nostd::get<int64_t>(value));
  }
  else if (nostd::holds_alternative<uint32_t>(value))
  {
    attribute->mutable_value()->set_int_value(nostd::get<uint32_t>(value));
  }
  else if (nostd::holds_alternative<uint64_t>(value))
  {
    attribute->mutable_value()->set_int_value(nostd::get<uint64_t>(value));
  }
  else if (nostd::holds_alternative<double>(value))
  {
    attribute->mutable_value()->set_double_value(nostd::get<double>(value));
  }
  else if (nostd::holds_alternative<std::string>(value))
  {
    attribute->mutable_value()->set_string_value(nostd::get<std::string>(value));
  }
  else if (nostd::holds_alternative<std::vector<bool>>(value))
  {
    for (const auto &val : nostd::get<std::vector<bool>>(value))
    {
      attribute->mutable_value()->mutable_array_value()->add_values()->set_bool_value(val);
    }
  }
  else if (nostd::holds_alternative<std::vector<int32_t>>(value))
  {
    for (const auto &val : nostd::get<std::vector<int32_t>>(value))
    {
      attribute->mutable_value()->mutable_array_value()->add_values()->set_int_value(val);
    }
  }
  else if (nostd::holds_alternative<std::vector<uint32_t>>(value))
  {
    for (const auto &val : nostd::get<std::vector<uint32_t>>(value))
    {
      attribute->mutable_value()->mutable_array_value()->add_values()->set_int_value(val);
    }
  }
  else if (nostd::holds_alternative<std::vector<int64_t>>(value))
  {
    for (const auto &val : nostd::get<std::vector<int64_t>>(value))
    {
      attribute->mutable_value()->mutable_array_value()->add_values()->set_int_value(val);
    }
  }
  else if (nostd::holds_alternative<std::vector<uint64_t>>(value))
  {
    for (const auto &val : nostd::get<std::vector<uint64_t>>(value))
    {
      attribute->mutable_value()->mutable_array_value()->add_values()->set_int_value(val);
    }
  }
  else if (nostd::holds_alternative<std::vector<double>>(value))
  {
    for (const auto &val : nostd::get<std::vector<double>>(value))
    {
      attribute->mutable_value()->mutable_array_value()->add_values()->set_double_value(val);
    }
  }
  else if (nostd::holds_alternative<std::vector<std::string>>(value))
  {
    for (const auto &val : nostd::get<std::vector<std::string>>(value))
    {
      attribute->mutable_value()->mutable_array_value()->add_values()->set_string_value(val);
    }
  }
}

proto::resource::v1::Resource OtlpRecordable::ProtoResource() const noexcept
{
  proto::resource::v1::Resource proto;
  if (resource_)
  {
    for (const auto &kv : resource_->GetAttributes())
    {
      PopulateAttribute(proto.add_attributes(), kv.first, kv.second);
    }
  }

  return proto;
}

void OtlpRecordable::SetResource(const opentelemetry::sdk::resource::Resource &resource) noexcept
{
  resource_ = &resource;
};

void OtlpRecordable::SetAttribute(nostd::string_view key,
                                  const opentelemetry::common::AttributeValue &value) noexcept
{
  auto *attribute = span_.add_attributes();
  PopulateAttribute(attribute, key, value);
}

void OtlpRecordable::AddEvent(nostd::string_view name,
                              common::SystemTimestamp timestamp,
                              const common::KeyValueIterable &attributes) noexcept
{
  auto *event = span_.add_events();
  event->set_name(name.data(), name.size());
  event->set_time_unix_nano(timestamp.time_since_epoch().count());

  attributes.ForEachKeyValue([&](nostd::string_view key, common::AttributeValue value) noexcept {
    PopulateAttribute(event->add_attributes(), key, value);
    return true;
  });
}

void OtlpRecordable::AddLink(const opentelemetry::trace::SpanContext &span_context,
                             const common::KeyValueIterable &attributes) noexcept
{
  auto *link = span_.add_links();
  link->set_trace_id(reinterpret_cast<const char *>(span_context.trace_id().Id().data()),
                     trace::TraceId::kSize);
  link->set_span_id(reinterpret_cast<const char *>(span_context.span_id().Id().data()),
                    trace::SpanId::kSize);
  attributes.ForEachKeyValue([&](nostd::string_view key, common::AttributeValue value) noexcept {
    PopulateAttribute(link->add_attributes(), key, value);
    return true;
  });

  // TODO: Populate trace_state when it is supported by SpanContext
}

void OtlpRecordable::SetStatus(trace::StatusCode code, nostd::string_view description) noexcept
{
  span_.mutable_status()->set_code(opentelemetry::proto::trace::v1::Status_StatusCode(code));
  span_.mutable_status()->set_message(description.data(), description.size());
}

void OtlpRecordable::SetName(nostd::string_view name) noexcept
{
  span_.set_name(name.data(), name.size());
}

void OtlpRecordable::SetSpanKind(opentelemetry::trace::SpanKind span_kind) noexcept
{
  opentelemetry::proto::trace::v1::Span_SpanKind proto_span_kind =
      opentelemetry::proto::trace::v1::Span_SpanKind::Span_SpanKind_SPAN_KIND_UNSPECIFIED;

  switch (span_kind)
  {

    case opentelemetry::trace::SpanKind::kInternal:
      proto_span_kind =
          opentelemetry::proto::trace::v1::Span_SpanKind::Span_SpanKind_SPAN_KIND_INTERNAL;
      break;

    case opentelemetry::trace::SpanKind::kServer:
      proto_span_kind =
          opentelemetry::proto::trace::v1::Span_SpanKind::Span_SpanKind_SPAN_KIND_SERVER;
      break;

    case opentelemetry::trace::SpanKind::kClient:
      proto_span_kind =
          opentelemetry::proto::trace::v1::Span_SpanKind::Span_SpanKind_SPAN_KIND_CLIENT;
      break;

    case opentelemetry::trace::SpanKind::kProducer:
      proto_span_kind =
          opentelemetry::proto::trace::v1::Span_SpanKind::Span_SpanKind_SPAN_KIND_PRODUCER;
      break;

    case opentelemetry::trace::SpanKind::kConsumer:
      proto_span_kind =
          opentelemetry::proto::trace::v1::Span_SpanKind::Span_SpanKind_SPAN_KIND_CONSUMER;
      break;

    default:
      // shouldn't reach here.
      proto_span_kind =
          opentelemetry::proto::trace::v1::Span_SpanKind::Span_SpanKind_SPAN_KIND_UNSPECIFIED;
  }

  span_.set_kind(proto_span_kind);
}

void OtlpRecordable::SetStartTime(opentelemetry::common::SystemTimestamp start_time) noexcept
{
  span_.set_start_time_unix_nano(start_time.time_since_epoch().count());
}

void OtlpRecordable::SetDuration(std::chrono::nanoseconds duration) noexcept
{
  const uint64_t unix_end_time = span_.start_time_unix_nano() + duration.count();
  span_.set_end_time_unix_nano(unix_end_time);
}

void OtlpRecordable::SetInstrumentationLibrary(
    const opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary
        &instrumentation_library) noexcept
{
  // TODO: add instrumentation library to OTLP exporter.
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
