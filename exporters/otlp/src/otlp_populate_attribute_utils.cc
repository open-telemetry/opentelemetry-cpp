// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#if defined(ENABLE_OTLP_UTF8_VALIDITY)
#  include <utf8_validity.h>
#endif

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <limits>
#include <new>
#include <ostream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/exporters/otlp/otlp_populate_attribute_utils.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/sdk/common/attribute_utils.h"
#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/instrumentationscope/instrumentation_scope.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/version.h"

// clang-format off
#include "opentelemetry/exporters/otlp/protobuf_include_prefix.h" // IWYU pragma: keep
#include <google/protobuf/repeated_ptr_field.h>
// IWYU pragma: no_include "net/proto2/public/repeated_field.h"
#include "opentelemetry/proto/common/v1/common.pb.h"
#include "opentelemetry/proto/resource/v1/resource.pb.h"
#include "opentelemetry/exporters/otlp/protobuf_include_suffix.h" // IWYU pragma: keep
// clang-format on

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

// Per OpenTelemetry spec, uint64_t attribute values exceeding INT64_MAX must be
// encoded as a decimal string rather than wrapping to a negative int64 via narrowing.
// https://opentelemetry.io/docs/specs/otel/common/attribute-type-mapping/#integer-values
inline void SetUint64Value(opentelemetry::proto::common::v1::AnyValue *proto_value, uint64_t val)
{
  if (val <= static_cast<uint64_t>(std::numeric_limits<int64_t>::max()))
  {
    proto_value->set_int_value(static_cast<int64_t>(val));
  }
  else
  {
    proto_value->set_string_value(std::to_string(val));
  }
}

template <class T>
struct IsSupportedAttributeValue : std::false_type
{};

struct AttributeValueVisitor
{
  opentelemetry::proto::common::v1::AnyValue *proto_value_;
  std::size_t max_length{std::numeric_limits<std::size_t>::max()};

  template <class T>
  void operator()(T &&) const
  {
    static_assert(
        IsSupportedAttributeValue<T>::value,
        "AttributeValueVisitor: Value type in opentelemetry::common::AttributeValue does not have "
        "an overload operator implemented in this visitor OR implicit conversion attempted!");
  }

  void operator()(bool value) const { proto_value_->set_bool_value(value); }
  void operator()(int32_t value) const { proto_value_->set_int_value(value); }
  void operator()(int64_t value) const { proto_value_->set_int_value(value); }
  void operator()(uint32_t value) const { proto_value_->set_int_value(value); }
  void operator()(uint64_t value) const { SetUint64Value(proto_value_, value); }
  void operator()(double value) const { proto_value_->set_double_value(value); }
  void operator()(const char *value) const
  {
    operator()(nostd::string_view{value, std::strlen(value)});
  }
  void operator()(nostd::string_view value) const
  {
    const std::size_t kept_len =
        OtlpPopulateAttributeUtils::Utf8SafePrefixLength(value.data(), value.size(), max_length);
#if defined(ENABLE_OTLP_UTF8_VALIDITY)
    if (utf8_range::IsStructurallyValid({value.data(), kept_len}))
    {
      proto_value_->set_string_value(value.data(), kept_len);
    }
    else
    {
      proto_value_->set_bytes_value(value.data(), kept_len);
    }
#else
    proto_value_->set_string_value(value.data(), kept_len);
#endif
  }
  void operator()(nostd::span<const bool> values) const
  {
    opentelemetry::proto::common::v1::ArrayValue *array_value = proto_value_->mutable_array_value();
    array_value->mutable_values()->Reserve(static_cast<int>(values.size()));
    for (const auto &val : values)
    {
      array_value->add_values()->set_bool_value(val);
    }
  }
  void operator()(nostd::span<const int32_t> values) const
  {
    opentelemetry::proto::common::v1::ArrayValue *array_value = proto_value_->mutable_array_value();
    array_value->mutable_values()->Reserve(static_cast<int>(values.size()));
    for (const auto &val : values)
    {
      array_value->add_values()->set_int_value(val);
    }
  }
  void operator()(nostd::span<const int64_t> values) const
  {
    opentelemetry::proto::common::v1::ArrayValue *array_value = proto_value_->mutable_array_value();
    array_value->mutable_values()->Reserve(static_cast<int>(values.size()));
    for (const auto &val : values)
    {
      array_value->add_values()->set_int_value(val);
    }
  }
  void operator()(nostd::span<const uint32_t> values) const
  {
    opentelemetry::proto::common::v1::ArrayValue *array_value = proto_value_->mutable_array_value();
    array_value->mutable_values()->Reserve(static_cast<int>(values.size()));
    for (const auto &val : values)
    {
      array_value->add_values()->set_int_value(val);
    }
  }
  void operator()(nostd::span<const double> values) const
  {
    opentelemetry::proto::common::v1::ArrayValue *array_value = proto_value_->mutable_array_value();
    array_value->mutable_values()->Reserve(static_cast<int>(values.size()));
    for (const auto &val : values)
    {
      array_value->add_values()->set_double_value(val);
    }
  }
  void operator()(nostd::span<const nostd::string_view> values) const
  {
    opentelemetry::proto::common::v1::ArrayValue *array_value = proto_value_->mutable_array_value();
    array_value->mutable_values()->Reserve(static_cast<int>(values.size()));
    for (const auto &val : values)
    {
      const std::size_t kept_len =
          OtlpPopulateAttributeUtils::Utf8SafePrefixLength(val.data(), val.size(), max_length);
#if defined(ENABLE_OTLP_UTF8_VALIDITY)
      if (utf8_range::IsStructurallyValid({val.data(), val.size()}))
      {
        array_value->add_values()->set_string_value(val.data(), kept_len);
      }
      else
      {
        array_value->add_values()->set_bytes_value(val.data(), kept_len);
      }
#else
      array_value->add_values()->set_string_value(val.data(), kept_len);
#endif
    }
  }
  void operator()(nostd::span<const uint64_t> values) const
  {
    opentelemetry::proto::common::v1::ArrayValue *array_value = proto_value_->mutable_array_value();
    array_value->mutable_values()->Reserve(static_cast<int>(values.size()));
    for (const auto &val : values)
    {
      SetUint64Value(array_value->add_values(), val);
    }
  }
  void operator()(nostd::span<const uint8_t> values) const
  {
    const auto &bytes          = values;
    const std::size_t kept_len = (std::min)(bytes.size(), max_length);
    proto_value_->set_bytes_value(reinterpret_cast<const void *>(bytes.data()), kept_len);
  }
};

struct OwnedAttributeValueVisitor
{
  opentelemetry::proto::common::v1::AnyValue *proto_value_;
  std::size_t max_length{std::numeric_limits<std::size_t>::max()};

  template <class T>
  void operator()(T &&) const
  {
    static_assert(IsSupportedAttributeValue<T>::value,
                  "OwnedAttributeValueVisitor: Value type in "
                  "opentelemetry::sdk::common::OwnedAttributeValue does not have an overload "
                  "operator implemented in this visitor OR implicit conversion attempted!");
  }

  void operator()(bool value) const { proto_value_->set_bool_value(value); }
  void operator()(int32_t value) const { proto_value_->set_int_value(value); }
  void operator()(uint32_t value) const { proto_value_->set_int_value(value); }
  void operator()(int64_t value) const { proto_value_->set_int_value(value); }
  void operator()(uint64_t value) const { SetUint64Value(proto_value_, value); }
  void operator()(double value) const { proto_value_->set_double_value(value); }
  void operator()(const std::string &value) const
  {
    const std::size_t kept_len =
        OtlpPopulateAttributeUtils::Utf8SafePrefixLength(value.data(), value.size(), max_length);
#if defined(ENABLE_OTLP_UTF8_VALIDITY)
    if (utf8_range::IsStructurallyValid({value.data(), kept_len}))
    {
      proto_value_->set_string_value(value.data(), kept_len);
    }
    else
    {
      proto_value_->set_bytes_value(value.data(), kept_len);
    }
#else
    proto_value_->set_string_value(value.data(), kept_len);
#endif
  }
  void operator()(const std::vector<bool> &values) const
  {
    opentelemetry::proto::common::v1::ArrayValue *array_value = proto_value_->mutable_array_value();
    array_value->mutable_values()->Reserve(static_cast<int>(values.size()));
    for (const auto val : values)
    {
      array_value->add_values()->set_bool_value(val);
    }
  }
  void operator()(const std::vector<int32_t> &values) const
  {
    opentelemetry::proto::common::v1::ArrayValue *array_value = proto_value_->mutable_array_value();
    array_value->mutable_values()->Reserve(static_cast<int>(values.size()));
    for (const auto &val : values)
    {
      array_value->add_values()->set_int_value(val);
    }
  }
  void operator()(const std::vector<uint32_t> &values) const
  {
    opentelemetry::proto::common::v1::ArrayValue *array_value = proto_value_->mutable_array_value();
    array_value->mutable_values()->Reserve(static_cast<int>(values.size()));
    for (const auto &val : values)
    {
      array_value->add_values()->set_int_value(static_cast<int64_t>(val));
    }
  }
  void operator()(const std::vector<int64_t> &values) const
  {
    opentelemetry::proto::common::v1::ArrayValue *array_value = proto_value_->mutable_array_value();
    array_value->mutable_values()->Reserve(static_cast<int>(values.size()));
    for (const auto &val : values)
    {
      array_value->add_values()->set_int_value(val);
    }
  }
  void operator()(const std::vector<double> &values) const
  {
    opentelemetry::proto::common::v1::ArrayValue *array_value = proto_value_->mutable_array_value();
    array_value->mutable_values()->Reserve(static_cast<int>(values.size()));
    for (const auto &val : values)
    {
      array_value->add_values()->set_double_value(val);
    }
  }
  void operator()(const std::vector<std::string> &values) const
  {
    opentelemetry::proto::common::v1::ArrayValue *array_value = proto_value_->mutable_array_value();
    array_value->mutable_values()->Reserve(static_cast<int>(values.size()));
    for (const auto &val : values)
    {
      const std::size_t kept_len =
          OtlpPopulateAttributeUtils::Utf8SafePrefixLength(val, max_length);
#if defined(ENABLE_OTLP_UTF8_VALIDITY)
      if (utf8_range::IsStructurallyValid({val.data(), kept_len}))
      {
        array_value->add_values()->set_string_value(val.data(), kept_len);
      }
      else
      {
        array_value->add_values()->set_bytes_value(val.data(), kept_len);
      }
#else
      array_value->add_values()->set_string_value(val.data(), kept_len);
#endif
    }
  }
  void operator()(const std::vector<uint64_t> &values) const
  {
    opentelemetry::proto::common::v1::ArrayValue *array_value = proto_value_->mutable_array_value();
    array_value->mutable_values()->Reserve(static_cast<int>(values.size()));
    for (const auto &val : values)
    {
      SetUint64Value(array_value->add_values(), val);
    }
  }
  void operator()(const std::vector<uint8_t> &values) const
  {
    proto_value_->set_bytes_value(reinterpret_cast<const void *>(values.data()), values.size());
  }
};

}  // namespace

void OtlpPopulateAttributeUtils::PopulateAnyValue(
    opentelemetry::proto::common::v1::AnyValue *proto_value,
    const opentelemetry::common::AttributeValue &value,
    AttributeValueMaxLength max_length) noexcept
{
#if OPENTELEMETRY_HAVE_EXCEPTIONS
  try
  {
#endif
    nostd::visit(AttributeValueVisitor{proto_value, max_length.value}, value);
#if OPENTELEMETRY_HAVE_EXCEPTIONS
  }
#  if defined(OPENTELEMETRY_HAVE_STD_VARIANT)
  catch (const std::bad_variant_access &e)
#  else
  catch (const opentelemetry::nostd::bad_variant_access &e)
#  endif
  {
    OTEL_INTERNAL_LOG_ERROR(
        "[OTLP Populate Attribute] bad_variant_access in PopulateAnyValue: " << e.what());
  }
  catch (const std::bad_alloc &)  // NOLINT(bugprone-empty-catch)
  {
    // TODO: Log an error once the logger doesn't require dynamic memory allocation.
    // OTEL_INTERNAL_LOG_ERROR(
    //     "[OTLP Populate Attribute] std::bad_alloc in PopulateAnyValue: " << e.what());
  }
#endif
}

void OtlpPopulateAttributeUtils::PopulateAnyValue(
    opentelemetry::proto::common::v1::AnyValue *proto_value,
    const opentelemetry::common::AttributeValue &value,
    bool /* allow_bytes */) noexcept
{
  PopulateAnyValue(proto_value, value, AttributeValueMaxLength{});
}

void OtlpPopulateAttributeUtils::PopulateAnyValue(
    opentelemetry::proto::common::v1::AnyValue *proto_value,
    const opentelemetry::sdk::common::OwnedAttributeValue &value,
    AttributeValueMaxLength max_length) noexcept
{
#if OPENTELEMETRY_HAVE_EXCEPTIONS
  try
  {
#endif
    nostd::visit(OwnedAttributeValueVisitor{proto_value, max_length.value}, value);
#if OPENTELEMETRY_HAVE_EXCEPTIONS
  }
#  if defined(OPENTELEMETRY_HAVE_STD_VARIANT)
  catch (const std::bad_variant_access &e)
#  else
  catch (const opentelemetry::nostd::bad_variant_access &e)
#  endif
  {
    OTEL_INTERNAL_LOG_ERROR(
        "[OTLP Populate Attribute] bad_variant_access in PopulateAnyValue: " << e.what());
  }
  catch (const std::bad_alloc &)  // NOLINT(bugprone-empty-catch)
  {
    // TODO: Log an error once the logger doesn't require dynamic memory allocation.
    // OTEL_INTERNAL_LOG_ERROR(
    //     "[OTLP Populate Attribute] std::bad_alloc in PopulateAnyValue: " << e.what());
  }
#endif
}

void OtlpPopulateAttributeUtils::PopulateAnyValue(
    opentelemetry::proto::common::v1::AnyValue *proto_value,
    const opentelemetry::sdk::common::OwnedAttributeValue &value,
    bool /* allow_bytes */) noexcept
{
  PopulateAnyValue(proto_value, value, AttributeValueMaxLength{});
}

void OtlpPopulateAttributeUtils::PopulateAttribute(
    opentelemetry::proto::common::v1::KeyValue *attribute,
    nostd::string_view key,
    const opentelemetry::common::AttributeValue &value,
    AttributeValueMaxLength max_length) noexcept
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
  PopulateAnyValue(attribute->mutable_value(), value, max_length);
}

void OtlpPopulateAttributeUtils::PopulateAttribute(
    opentelemetry::proto::common::v1::KeyValue *attribute,
    nostd::string_view key,
    const opentelemetry::common::AttributeValue &value,
    bool /* allow_bytes */) noexcept
{
  PopulateAttribute(attribute, key, value, AttributeValueMaxLength{});
}

/** Maps from C++ attribute into OTLP proto attribute. */
void OtlpPopulateAttributeUtils::PopulateAttribute(
    opentelemetry::proto::common::v1::KeyValue *attribute,
    nostd::string_view key,
    const opentelemetry::sdk::common::OwnedAttributeValue &value,
    AttributeValueMaxLength max_length) noexcept
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
  PopulateAnyValue(attribute->mutable_value(), value, max_length);
}

void OtlpPopulateAttributeUtils::PopulateAttribute(
    opentelemetry::proto::common::v1::KeyValue *attribute,
    nostd::string_view key,
    const opentelemetry::sdk::common::OwnedAttributeValue &value,
    bool /* allow_bytes */) noexcept
{
  PopulateAttribute(attribute, key, value, AttributeValueMaxLength{});
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
                                                  AttributeValueMaxLength{});
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
                                                  AttributeValueMaxLength{});
  }
}

// The UTF-8-safe prefix algorithm now lives in the SDK common layer so the
// in-memory AttributeConverter can share it. This member is kept as a thin
// forwarder for backward compatibility with existing callers and tests.
std::size_t OtlpPopulateAttributeUtils::Utf8SafePrefixLength(const char *data,
                                                             std::size_t size,
                                                             std::size_t max_bytes) noexcept
{
  return opentelemetry::sdk::common::Utf8SafePrefixLength(data, size, max_bytes);
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
