#include "exporters/otlp/recordable.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{
void Recordable::SetIds(trace::TraceId trace_id,
                        trace::SpanId span_id,
                        trace::SpanId parent_span_id) noexcept
{
  span_.set_trace_id(reinterpret_cast<const char *>(trace_id.Id().data()), trace::TraceId::kSize);
  span_.set_span_id(reinterpret_cast<const char *>(span_id.Id().data()), trace::SpanId::kSize);
  span_.set_parent_span_id(reinterpret_cast<const char *>(parent_span_id.Id().data()),
                           trace::SpanId::kSize);
}

void Recordable::SetAttribute(nostd::string_view key,
                              const opentelemetry::common::AttributeValue &value) noexcept
{
  auto attribute = span_.add_attributes();
  attribute->set_key(key.data(), key.size());

  // Cases correspond to nostd::variant in AttributeValue
  switch (value.index())
  {
    case 0:
      attribute->mutable_value()->set_bool_value(nostd::get<bool>(value));
      break;
    case 1:
      attribute->mutable_value()->set_int_value(nostd::get<int>(value));
      break;
    case 2:
      attribute->mutable_value()->set_int_value(nostd::get<int64_t>(value));
      break;
    case 3:
      attribute->mutable_value()->set_int_value(nostd::get<unsigned int>(value));
      break;
    case 4:
      attribute->mutable_value()->set_int_value(nostd::get<uint64_t>(value));
      break;
    case 5:
      attribute->mutable_value()->set_double_value(nostd::get<double>(value));
      break;
    case 6:
      attribute->mutable_value()->set_string_value(nostd::get<nostd::string_view>(value).data(),
                                                   nostd::get<nostd::string_view>(value).size());
      break;
    case 7:
      for (auto &val : nostd::get<nostd::span<const bool>>(value))
      {
        attribute->mutable_value()->mutable_array_value()->add_values()->set_bool_value(val);
      }
      break;
    case 8:
      for (auto &val : nostd::get<nostd::span<const int>>(value))
      {
        attribute->mutable_value()->mutable_array_value()->add_values()->set_int_value(val);
      }
      break;
    case 9:
      for (auto &val : nostd::get<nostd::span<const int64_t>>(value))
      {
        attribute->mutable_value()->mutable_array_value()->add_values()->set_int_value(val);
      }
      break;
    case 10:
      for (auto &val : nostd::get<nostd::span<const unsigned int>>(value))
      {
        attribute->mutable_value()->mutable_array_value()->add_values()->set_int_value(val);
      }
      break;
    case 11:
      for (auto &val : nostd::get<nostd::span<const uint64_t>>(value))
      {
        attribute->mutable_value()->mutable_array_value()->add_values()->set_int_value(val);
      }
      break;
    case 12:
      for (auto &val : nostd::get<nostd::span<const double>>(value))
      {
        attribute->mutable_value()->mutable_array_value()->add_values()->set_double_value(val);
      }
      break;
    case 13:
      for (auto &val : nostd::get<nostd::span<const nostd::string_view>>(value))
      {
        attribute->mutable_value()->mutable_array_value()->add_values()->set_string_value(
            val.data(), val.size());
      }
      break;
  }
}

void Recordable::AddEvent(nostd::string_view name, core::SystemTimestamp timestamp) noexcept
{
  (void)name;
}

void Recordable::SetStatus(trace::CanonicalCode code, nostd::string_view description) noexcept
{
  span_.mutable_status()->set_code(opentelemetry::proto::trace::v1::Status_StatusCode(code));
  span_.mutable_status()->set_message(description.data(), description.size());
}

void Recordable::SetName(nostd::string_view name) noexcept
{
  span_.set_name(name.data(), name.size());
}

void Recordable::SetStartTime(opentelemetry::core::SystemTimestamp start_time) noexcept
{
  const uint64_t nano_unix_time = start_time.time_since_epoch().count();
  span_.set_start_time_unix_nano(nano_unix_time);
}

void Recordable::SetDuration(std::chrono::nanoseconds duration) noexcept
{
  const uint64_t unix_end_time = span_.start_time_unix_nano() + duration.count();
  span_.set_end_time_unix_nano(unix_end_time);
}
}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
