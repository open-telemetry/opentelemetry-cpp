#include "opentelemetry/exporters/otlp/shared_utils.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{
namespace internal
{

const int kAttributeValueSize = 14;
#ifdef HAVE_SPAN_BYTE
const int kOwnedAttributeValueSize = 15;
#else
const int kOwnedAttributeValueSize = 14;
#endif

/** Maps from C++ attribute into OTLP proto attribute. */
void PopulateAttribute(opentelemetry::proto::common::v1::KeyValue *attribute,
                       nostd::string_view key,
                       const opentelemetry::common::AttributeValue &value) {
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
#ifdef HAVE_CSTRING_TYPE
  else if (nostd::holds_alternative<const char *>(value))
  {
    attribute->mutable_value()->set_string_value(nostd::get<const char *>(value));
  }
#endif
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
                       const sdk::common::OwnedAttributeValue &value) {
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
#ifdef HAVE_SPAN_BYTE
  else if (nostd::holds_alternative<uint8_t>(value))
  {
    attribute->mutable_value()->set_int_value(nostd::get<uint8_t>(value));
  }
#endif
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

}  // namespace internal
}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE