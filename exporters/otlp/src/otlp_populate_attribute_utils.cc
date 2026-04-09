// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

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

  void operator()(bool v) const noexcept { proto_value_->set_bool_value(v); }
  void operator()(int32_t v) const noexcept { proto_value_->set_int_value(v); }
  void operator()(int64_t v) const noexcept { proto_value_->set_int_value(v); }
  void operator()(uint32_t v) const noexcept { proto_value_->set_int_value(v); }
  void operator()(double v) const noexcept { proto_value_->set_double_value(v); }
  void operator()(const char *v) const noexcept { proto_value_->set_string_value(v); }
  void operator()(nostd::string_view v) const noexcept
  {
    proto_value_->set_string_value(v.data(), v.size());
  }
  void operator()(nostd::span<const bool> v) const noexcept
  {
    opentelemetry::proto::common::v1::ArrayValue *array_value = proto_value_->mutable_array_value();
    array_value->mutable_values()->Reserve(static_cast<int>(v.size()));
    for (const auto &val : v)
    {
      array_value->add_values()->set_bool_value(val);
    }
  }
  void operator()(nostd::span<const int32_t> v) const noexcept
  {
    opentelemetry::proto::common::v1::ArrayValue *array_value = proto_value_->mutable_array_value();
    array_value->mutable_values()->Reserve(static_cast<int>(v.size()));
    for (const auto &val : v)
    {
      array_value->add_values()->set_int_value(val);
    }
  }
  void operator()(nostd::span<const int64_t> v) const noexcept
  {
    opentelemetry::proto::common::v1::ArrayValue *array_value = proto_value_->mutable_array_value();
    array_value->mutable_values()->Reserve(static_cast<int>(v.size()));
    for (const auto &val : v)
    {
      array_value->add_values()->set_int_value(val);
    }
  }
  void operator()(nostd::span<const uint32_t> v) const noexcept
  {
    opentelemetry::proto::common::v1::ArrayValue *array_value = proto_value_->mutable_array_value();
    array_value->mutable_values()->Reserve(static_cast<int>(v.size()));
    for (const auto &val : v)
    {
      array_value->add_values()->set_int_value(val);
    }
  }
  void operator()(nostd::span<const double> v) const noexcept
  {
    opentelemetry::proto::common::v1::ArrayValue *array_value = proto_value_->mutable_array_value();
    array_value->mutable_values()->Reserve(static_cast<int>(v.size()));
    for (const auto &val : v)
    {
      array_value->add_values()->set_double_value(val);
    }
  }
  void operator()(nostd::span<const nostd::string_view> v) const noexcept
  {
    opentelemetry::proto::common::v1::ArrayValue *array_value = proto_value_->mutable_array_value();
    array_value->mutable_values()->Reserve(static_cast<int>(v.size()));
    for (const auto &val : v)
    {
      array_value->add_values()->set_string_value(val.data(), val.size());
    }
  }
  void operator()(uint64_t v) const noexcept
  {
    // Values within int64_t range map to int_value and string if out of range per the `Mapping
    // Arbitrary Data to OTLP AnyValue` specification.
    if (v <= static_cast<uint64_t>(std::numeric_limits<int64_t>::max()))
    {
      proto_value_->set_int_value(static_cast<int64_t>(v));
    }
    else
    {
      proto_value_->set_string_value(std::to_string(v));
    }
  }
  void operator()(nostd::span<const uint64_t> v) const noexcept
  {
    // TODO: need to handle uint64_t values above int64_t range per the `Mapping Arbitrary Data to
    // OTLP AnyValue` specification. A homogeneous array MUST NOT contain values of different types.
    opentelemetry::proto::common::v1::ArrayValue *array_value = proto_value_->mutable_array_value();
    array_value->mutable_values()->Reserve(static_cast<int>(v.size()));
    for (const auto &val : v)
    {
      array_value->add_values()->set_int_value(static_cast<int64_t>(val));
    }
  }
  void operator()(nostd::span<const uint8_t> v) const noexcept
  {
    if (allow_bytes_)
    {
      proto_value_->set_bytes_value(reinterpret_cast<const void *>(v.data()), v.size());
    }
    else
    {
      opentelemetry::proto::common::v1::ArrayValue *array_value =
          proto_value_->mutable_array_value();
      array_value->mutable_values()->Reserve(static_cast<int>(v.size()));
      for (const auto &val : v)
      {
        array_value->add_values()->set_int_value(val);
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

  void operator()(bool v) const noexcept { proto_value_->set_bool_value(v); }
  void operator()(int32_t v) const noexcept { proto_value_->set_int_value(v); }
  void operator()(uint32_t v) const noexcept { proto_value_->set_int_value(v); }
  void operator()(int64_t v) const noexcept { proto_value_->set_int_value(v); }
  void operator()(double v) const noexcept { proto_value_->set_double_value(v); }
  void operator()(const std::string &v) const noexcept { proto_value_->set_string_value(v); }
  void operator()(const std::vector<bool> &v) const noexcept
  {
    opentelemetry::proto::common::v1::ArrayValue *array_value = proto_value_->mutable_array_value();
    array_value->mutable_values()->Reserve(static_cast<int>(v.size()));
    for (const auto val : v)
    {
      array_value->add_values()->set_bool_value(val);
    }
  }
  void operator()(const std::vector<int32_t> &v) const noexcept
  {
    opentelemetry::proto::common::v1::ArrayValue *array_value = proto_value_->mutable_array_value();
    array_value->mutable_values()->Reserve(static_cast<int>(v.size()));
    for (const auto &val : v)
    {
      array_value->add_values()->set_int_value(val);
    }
  }
  void operator()(const std::vector<uint32_t> &v) const noexcept
  {
    opentelemetry::proto::common::v1::ArrayValue *array_value = proto_value_->mutable_array_value();
    array_value->mutable_values()->Reserve(static_cast<int>(v.size()));
    for (const auto &val : v)
    {
      array_value->add_values()->set_int_value(static_cast<int64_t>(val));
    }
  }
  void operator()(const std::vector<int64_t> &v) const noexcept
  {
    opentelemetry::proto::common::v1::ArrayValue *array_value = proto_value_->mutable_array_value();
    array_value->mutable_values()->Reserve(static_cast<int>(v.size()));
    for (const auto &val : v)
    {
      array_value->add_values()->set_int_value(val);
    }
  }
  void operator()(const std::vector<double> &v) const noexcept
  {
    opentelemetry::proto::common::v1::ArrayValue *array_value = proto_value_->mutable_array_value();
    array_value->mutable_values()->Reserve(static_cast<int>(v.size()));
    for (const auto &val : v)
    {
      array_value->add_values()->set_double_value(val);
    }
  }
  void operator()(const std::vector<std::string> &v) const noexcept
  {
    opentelemetry::proto::common::v1::ArrayValue *array_value = proto_value_->mutable_array_value();
    array_value->mutable_values()->Reserve(static_cast<int>(v.size()));
    for (const auto &val : v)
    {
      array_value->add_values()->set_string_value(val);
    }
  }
  void operator()(uint64_t v) const noexcept
  {
    // Values within int64_t range map to int_value and string if out of range per the `Mapping
    // Arbitrary Data to OTLP AnyValue` specification.
    if (v <= static_cast<uint64_t>(std::numeric_limits<int64_t>::max()))
    {
      proto_value_->set_int_value(static_cast<int64_t>(v));
    }
    else
    {
      proto_value_->set_string_value(std::to_string(v));
    }
  }
  void operator()(const std::vector<uint64_t> &v) const noexcept
  {
    // TODO: need to handle uint64_t values above int64_t range per the `Mapping Arbitrary Data to
    // OTLP AnyValue` specification. A homogeneous array MUST NOT contain values of different types.
    opentelemetry::proto::common::v1::ArrayValue *array_value = proto_value_->mutable_array_value();
    array_value->mutable_values()->Reserve(static_cast<int>(v.size()));
    for (const auto &val : v)
    {
      array_value->add_values()->set_int_value(static_cast<int64_t>(val));
    }
  }
  void operator()(const std::vector<uint8_t> &v) const noexcept
  {
    if (allow_bytes_)
    {
      proto_value_->set_bytes_value(reinterpret_cast<const void *>(v.data()), v.size());
    }
    else
    {
      opentelemetry::proto::common::v1::ArrayValue *array_value =
          proto_value_->mutable_array_value();
      array_value->mutable_values()->Reserve(static_cast<int>(v.size()));
      for (const auto &val : v)
      {
        array_value->add_values()->set_int_value(val);
      }
    }
  }
};

}  // namespace

void OtlpPopulateAttributeUtils::PopulateAnyValue(
    opentelemetry::proto::common::v1::AnyValue *proto_value,
    const opentelemetry::common::AttributeValue &value,
    bool allow_bytes) noexcept
{
  // AttributeValueVisitor is noexcept and nostd::visit should never throw.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  nostd::visit(AttributeValueVisitor{proto_value, allow_bytes}, value);
}

void OtlpPopulateAttributeUtils::PopulateAnyValue(
    opentelemetry::proto::common::v1::AnyValue *proto_value,
    const opentelemetry::sdk::common::OwnedAttributeValue &value,
    bool allow_bytes) noexcept
{
  // OwnedAttributeValueVisitor is noexcept and nostd::visit should never throw.
  // NOLINTNEXTLINE(bugprone-exception-escape)
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
