// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#if defined(ENABLE_OTLP_UTF8_VALIDITY)
#  include <utf8_validity.h>
#endif

#include <cstdint>
#include <limits>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/exporters/otlp/otlp_populate_attribute_utils.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/sdk/common/attribute_utils.h"
#include "opentelemetry/sdk/instrumentationscope/instrumentation_scope.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/version.h"

// clang-format off
#include "opentelemetry/exporters/otlp/protobuf_include_prefix.h" // IWYU pragma: keep
// IWYU pragma: no_include "net/proto2/public/repeated_field.h"
#include "opentelemetry/proto/common/v1/common.pb.h"
#include "opentelemetry/proto/resource/v1/resource.pb.h"
#include "opentelemetry/exporters/otlp/protobuf_include_suffix.h" // IWYU pragma: keep
// clang-format on

namespace nostd = opentelemetry::nostd;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

namespace
{

//
// See `attribute_value.h` for details.
//
const int kAttributeValueSize      = 16;
const int kOwnedAttributeValueSize = 15;

template <class T>
struct IsSupportedAttributeValue : std::false_type
{};

struct AttributeValueVisitor
{
  opentelemetry::proto::common::v1::AnyValue *proto_value_;
  bool allow_bytes_;

  template <class T>
  void operator()(T &&) const noexcept
  {
    static_assert(
        IsSupportedAttributeValue<T>::value,
        "AttributeValueVisitor: Value type in opentelemetry::common::AttributeValue does not have "
        "an overload operator implemented in this visitor OR implicit conversion attempted!");
  }

  void operator()(bool bool_value) const noexcept { proto_value_->set_bool_value(bool_value); }
  void operator()(int32_t int32_value) const noexcept { proto_value_->set_int_value(int32_value); }
  void operator()(int64_t int64_value) const noexcept { proto_value_->set_int_value(int64_value); }
  void operator()(uint32_t uint32_value) const noexcept
  {
    proto_value_->set_int_value(uint32_value);
  }
  void operator()(double double_value) const noexcept
  {
    proto_value_->set_double_value(double_value);
  }
  void operator()(const char *str_value) const noexcept
  {
#if defined(ENABLE_OTLP_UTF8_VALIDITY)
    if (utf8_range::IsStructurallyValid(str_value))
    {
      proto_value_->set_string_value(str_value);
    }
    else
    {
      proto_value_->set_bytes_value(str_value, strlen(str_value));
    }
#else
    proto_value_->set_string_value(str_value);
#endif
  }
  void operator()(nostd::string_view str_value) const noexcept
  {
#if defined(ENABLE_OTLP_UTF8_VALIDITY)
    if (utf8_range::IsStructurallyValid({str_value.data(), str_value.size()}))
    {
      proto_value_->set_string_value(str_value.data(), str_value.size());
    }
    else
    {
      proto_value_->set_bytes_value(str_value.data(), str_value.size());
    }
#else
    proto_value_->set_string_value(str_value.data(), str_value.size());
#endif
  }
  void operator()(nostd::span<const bool> span_bool_value) const noexcept
  {
    opentelemetry::proto::common::v1::ArrayValue *array_value = proto_value_->mutable_array_value();
    array_value->mutable_values()->Reserve(static_cast<int>(span_bool_value.size()));
    for (const auto &val : span_bool_value)
    {
      array_value->add_values()->set_bool_value(val);
    }
  }
  void operator()(nostd::span<const int32_t> span_int32_value) const noexcept
  {
    opentelemetry::proto::common::v1::ArrayValue *array_value = proto_value_->mutable_array_value();
    array_value->mutable_values()->Reserve(static_cast<int>(span_int32_value.size()));
    for (const auto &val : span_int32_value)
    {
      array_value->add_values()->set_int_value(val);
    }
  }
  void operator()(nostd::span<const int64_t> span_int64_value) const noexcept
  {
    opentelemetry::proto::common::v1::ArrayValue *array_value = proto_value_->mutable_array_value();
    array_value->mutable_values()->Reserve(static_cast<int>(span_int64_value.size()));
    for (const auto &val : span_int64_value)
    {
      array_value->add_values()->set_int_value(val);
    }
  }
  void operator()(nostd::span<const uint32_t> span_uint32_value) const noexcept
  {
    opentelemetry::proto::common::v1::ArrayValue *array_value = proto_value_->mutable_array_value();
    array_value->mutable_values()->Reserve(static_cast<int>(span_uint32_value.size()));
    for (const auto &uint32_value : span_uint32_value)
    {
      array_value->add_values()->set_int_value(uint32_value);
    }
  }
  void operator()(nostd::span<const double> span_double_value) const noexcept
  {
    opentelemetry::proto::common::v1::ArrayValue *array_value = proto_value_->mutable_array_value();
    array_value->mutable_values()->Reserve(static_cast<int>(span_double_value.size()));
    for (const auto &double_value : span_double_value)
    {
      array_value->add_values()->set_double_value(double_value);
    }
  }
  void operator()(nostd::span<const nostd::string_view> span_string_value) const noexcept
  {
    opentelemetry::proto::common::v1::ArrayValue *array_value = proto_value_->mutable_array_value();
    array_value->mutable_values()->Reserve(static_cast<int>(span_string_value.size()));
    for (const auto &string_value : span_string_value)
    {
#if defined(ENABLE_OTLP_UTF8_VALIDITY)
      if (utf8_range::IsStructurallyValid({string_value.data(), string_value.size()}))
      {
        array_value->add_values()->set_string_value(string_value.data(), string_value.size());
      }
      else
      {
        array_value->add_values()->set_bytes_value(string_value.data(), string_value.size());
      }
#else
      array_value->add_values()->set_string_value(string_value.data(), string_value.size());
#endif
    }
  }
  void operator()(uint64_t uint64_value) const noexcept
  {
    // Values within int64_t range map to int_value and string if out of range per the `Mapping
    // Arbitrary Data to OTLP AnyValue` specification.
    if (uint64_value <= static_cast<uint64_t>(std::numeric_limits<int64_t>::max()))
    {
      proto_value_->set_int_value(static_cast<int64_t>(uint64_value));
    }
    else
    {
      proto_value_->set_string_value(std::to_string(uint64_value));
    }
  }
  void operator()(nostd::span<const uint64_t> span_uint64_value) const noexcept
  {
    opentelemetry::proto::common::v1::ArrayValue *array_value = proto_value_->mutable_array_value();
    array_value->mutable_values()->Reserve(static_cast<int>(span_uint64_value.size()));
    for (const auto &uint64_value : span_uint64_value)
    {
      // Values within int64_t range map to int_value and string if out of range per the `Mapping
      // Arbitrary Data to OTLP AnyValue` specification.
      if (uint64_value <= static_cast<uint64_t>(std::numeric_limits<int64_t>::max()))
      {
        array_value->add_values()->set_int_value(static_cast<int64_t>(uint64_value));
      }
      else
      {
        array_value->add_values()->set_string_value(std::to_string(uint64_value));
      }
    }
  }
  void operator()(nostd::span<const uint8_t> span_uint8_value) const noexcept
  {
    if (allow_bytes_)
    {
      proto_value_->set_bytes_value(reinterpret_cast<const void *>(span_uint8_value.data()),
                                    span_uint8_value.size());
    }
    else
    {
      opentelemetry::proto::common::v1::ArrayValue *array_value =
          proto_value_->mutable_array_value();
      array_value->mutable_values()->Reserve(static_cast<int>(span_uint8_value.size()));
      for (const auto &uint8_value : span_uint8_value)
      {
        array_value->add_values()->set_int_value(uint8_value);
      }
    }
  }
};

struct OwnedAttributeValueVisitor
{
  opentelemetry::proto::common::v1::AnyValue *proto_value_;
  bool allow_bytes_;

  template <class T>
  void operator()(T &&) const noexcept
  {
    static_assert(IsSupportedAttributeValue<T>::value,
                  "OwnedAttributeValueVisitor: Value type in "
                  "opentelemetry::sdk::common::OwnedAttributeValue does not have an overload "
                  "operator implemented in this visitor OR implicit conversion attempted!");
  }

  void operator()(bool bool_value) const noexcept { proto_value_->set_bool_value(bool_value); }
  void operator()(int32_t int32_value) const noexcept { proto_value_->set_int_value(int32_value); }
  void operator()(uint32_t uint32_value) const noexcept
  {
    proto_value_->set_int_value(uint32_value);
  }
  void operator()(int64_t int64_value) const noexcept { proto_value_->set_int_value(int64_value); }
  void operator()(double double_value) const noexcept
  {
    proto_value_->set_double_value(double_value);
  }
  void operator()(const std::string &string_value) const noexcept
  {
    proto_value_->set_string_value(string_value);
  }
  void operator()(const std::vector<bool> &vector_bool_value) const noexcept
  {
    opentelemetry::proto::common::v1::ArrayValue *array_value = proto_value_->mutable_array_value();
    array_value->mutable_values()->Reserve(static_cast<int>(vector_bool_value.size()));
    for (const auto bool_value : vector_bool_value)
    {
      array_value->add_values()->set_bool_value(bool_value);
    }
  }
  void operator()(const std::vector<int32_t> &vector_int32_value) const noexcept
  {
    opentelemetry::proto::common::v1::ArrayValue *array_value = proto_value_->mutable_array_value();
    array_value->mutable_values()->Reserve(static_cast<int>(vector_int32_value.size()));
    for (const auto &int32_value : vector_int32_value)
    {
      array_value->add_values()->set_int_value(int32_value);
    }
  }
  void operator()(const std::vector<uint32_t> &vector_uint32_value) const noexcept
  {
    opentelemetry::proto::common::v1::ArrayValue *array_value = proto_value_->mutable_array_value();
    array_value->mutable_values()->Reserve(static_cast<int>(vector_uint32_value.size()));
    for (const auto &uint32_value : vector_uint32_value)
    {
      array_value->add_values()->set_int_value(static_cast<int64_t>(uint32_value));
    }
  }
  void operator()(const std::vector<int64_t> &vector_int64_value) const noexcept
  {
    opentelemetry::proto::common::v1::ArrayValue *array_value = proto_value_->mutable_array_value();
    array_value->mutable_values()->Reserve(static_cast<int>(vector_int64_value.size()));
    for (const auto &int64_value : vector_int64_value)
    {
      array_value->add_values()->set_int_value(int64_value);
    }
  }
  void operator()(const std::vector<double> &vector_double_value) const noexcept
  {
    opentelemetry::proto::common::v1::ArrayValue *array_value = proto_value_->mutable_array_value();
    array_value->mutable_values()->Reserve(static_cast<int>(vector_double_value.size()));
    for (const auto &double_value : vector_double_value)
    {
      array_value->add_values()->set_double_value(double_value);
    }
  }
  void operator()(const std::vector<std::string> &vector_string_value) const noexcept
  {
    opentelemetry::proto::common::v1::ArrayValue *array_value = proto_value_->mutable_array_value();
    array_value->mutable_values()->Reserve(static_cast<int>(vector_string_value.size()));
    for (const auto &string_value : vector_string_value)
    {
#if defined(ENABLE_OTLP_UTF8_VALIDITY)
      if (utf8_range::IsStructurallyValid({string_value.data(), string_value.size()}))
      {
        array_value->add_values()->set_string_value(string_value.data(), string_value.size());
      }
      else
      {
        array_value->add_values()->set_bytes_value(string_value.data(), string_value.size());
      }
#else
      array_value->add_values()->set_string_value(string_value.data(), string_value.size());
#endif
    }
  }
  void operator()(uint64_t uint64_value) const noexcept
  {
    // Values within int64_t range map to int_value and string if out of range per the `Mapping
    // Arbitrary Data to OTLP AnyValue` specification.
    if (uint64_value <= static_cast<uint64_t>(std::numeric_limits<int64_t>::max()))
    {
      proto_value_->set_int_value(static_cast<int64_t>(uint64_value));
    }
    else
    {
      proto_value_->set_string_value(std::to_string(uint64_value));
    }
  }
  void operator()(const std::vector<uint64_t> &vector_uint64_value) const noexcept
  {
    // TODO: need to handle uint64_t values above int64_t range per the `Mapping Arbitrary Data to
    // OTLP AnyValue` specification. A homogeneous array MUST NOT contain values of different types.
    opentelemetry::proto::common::v1::ArrayValue *array_value = proto_value_->mutable_array_value();
    array_value->mutable_values()->Reserve(static_cast<int>(vector_uint64_value.size()));
    for (const auto &uint64_value : vector_uint64_value)
    {
      array_value->add_values()->set_int_value(static_cast<int64_t>(uint64_value));
    }
  }
  void operator()(const std::vector<uint8_t> &vector_uint8_value) const noexcept
  {
    if (allow_bytes_)
    {
      proto_value_->set_bytes_value(reinterpret_cast<const void *>(vector_uint8_value.data()),
                                    vector_uint8_value.size());
    }
    else
    {
      opentelemetry::proto::common::v1::ArrayValue *array_value =
          proto_value_->mutable_array_value();
      array_value->mutable_values()->Reserve(static_cast<int>(vector_uint8_value.size()));
      for (const auto &uint8_value : vector_uint8_value)
      {
        array_value->add_values()->set_int_value(uint8_value);
      }
    }
  }
};

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
void OtlpPopulateAttributeUtils::PopulateAnyValue(
    opentelemetry::proto::common::v1::AnyValue *proto_value,
    const opentelemetry::common::AttributeValue &value,
    bool allow_bytes) noexcept
{
  // AttributeValueVisitor is noexcept and nostd::visit should never throw.
  nostd::visit(AttributeValueVisitor{proto_value, allow_bytes}, value);
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void OtlpPopulateAttributeUtils::PopulateAnyValue(
    opentelemetry::proto::common::v1::AnyValue *proto_value,
    const opentelemetry::sdk::common::OwnedAttributeValue &value,
    bool allow_bytes) noexcept
{
  // OwnedAttributeValueVisitor is noexcept and nostd::visit should never throw.
  nostd::visit(OwnedAttributeValueVisitor{proto_value, allow_bytes}, value);
}

void OtlpPopulateAttributeUtils::PopulateAttribute(
    opentelemetry::proto::common::v1::KeyValue *attribute,
    nostd::string_view key,
    const opentelemetry::common::AttributeValue &value,
    bool allow_bytes) noexcept
{
  if (nullptr == attribute)
  {
    return;
  }

  // Assert size of variant to ensure that this method gets updated if the variant
  // definition changes
  static_assert(
      nostd::variant_size<opentelemetry::common::AttributeValue>::value == kAttributeValueSize,
      "AttributeValue contains unknown type");

  attribute->set_key(key.data(), key.size());
  PopulateAnyValue(attribute->mutable_value(), value, allow_bytes);
}

/** Maps from C++ attribute into OTLP proto attribute. */
void OtlpPopulateAttributeUtils::PopulateAttribute(
    opentelemetry::proto::common::v1::KeyValue *attribute,
    nostd::string_view key,
    const opentelemetry::sdk::common::OwnedAttributeValue &value,
    bool allow_bytes) noexcept
{
  if (nullptr == attribute)
  {
    return;
  }

  // Assert size of variant to ensure that this method gets updated if the variant
  // definition changes
  static_assert(nostd::variant_size<opentelemetry::sdk::common::OwnedAttributeValue>::value ==
                    kOwnedAttributeValueSize,
                "OwnedAttributeValue contains unknown type");

  attribute->set_key(key.data(), key.size());
  PopulateAnyValue(attribute->mutable_value(), value, allow_bytes);
}

void OtlpPopulateAttributeUtils::PopulateAttribute(
    opentelemetry::proto::resource::v1::Resource *proto,
    const opentelemetry::sdk::resource::Resource &resource) noexcept
{
  if (nullptr == proto)
  {
    return;
  }

  for (const auto &kv : resource.GetAttributes())
  {
    OtlpPopulateAttributeUtils::PopulateAttribute(proto->add_attributes(), kv.first, kv.second,
                                                  false);
  }
}

void OtlpPopulateAttributeUtils::PopulateAttribute(
    opentelemetry::proto::common::v1::InstrumentationScope *proto,
    const opentelemetry::sdk::instrumentationscope::InstrumentationScope
        &instrumentation_scope) noexcept
{
  for (const auto &kv : instrumentation_scope.GetAttributes())
  {
    OtlpPopulateAttributeUtils::PopulateAttribute(proto->add_attributes(), kv.first, kv.second,
                                                  false);
  }
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
