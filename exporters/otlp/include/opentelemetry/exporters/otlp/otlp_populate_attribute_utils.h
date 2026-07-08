// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>
#include <limits>
#include <string>

#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/common/macros.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/common/attribute_utils.h"
#include "opentelemetry/sdk/instrumentationscope/instrumentation_scope.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/version.h"

namespace opentelemetry
{
namespace proto
{

namespace common
{
namespace v1
{
class AnyValue;
class KeyValue;
class InstrumentationScope;
}  // namespace v1
}  // namespace common

namespace resource
{
namespace v1
{
class Resource;
}
}  // namespace resource

}  // namespace proto
}  // namespace opentelemetry

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

/**
 *  Attribute Value Max Length accepted by OtlpPopulateAttributeUtils::PopulateAnyValue and
 * OtlpPopulateAttributeUtils::PopulateAttribute. Defaults to max of size_t (no limit).
 */
struct AttributeValueMaxLength
{
  std::size_t value{(std::numeric_limits<std::size_t>::max)()};
  AttributeValueMaxLength() noexcept = default;
  explicit AttributeValueMaxLength(std::size_t v) noexcept : value(v) {}
};

/**
 * The OtlpPopulateAttributeUtils contains utility functions to populate attributes
 */
class OtlpPopulateAttributeUtils
{

public:
  static void PopulateAttribute(opentelemetry::proto::resource::v1::Resource *proto,
                                const opentelemetry::sdk::resource::Resource &resource) noexcept;

  static void PopulateAttribute(opentelemetry::proto::common::v1::InstrumentationScope *proto,
                                const opentelemetry::sdk::instrumentationscope::InstrumentationScope
                                    &instrumentation_scope) noexcept;

  /**
   * Populate a proto AnyValue from a non-owning AttributeValue.
   * When `max_length` is less than `std::numeric_limits<std::size_t>::max()`,
   * string alternatives are truncated to at most `max_length` bytes using
   * UTF-8-safe truncation (Utf8SafePrefixLength) so the resulting proto
   * `string_value` stays valid UTF-8 when the input was. Raw bytes
   * (span<const uint8_t>) are truncated at the raw byte boundary. Non-string
   * alternatives are unaffected.
   */
  static void PopulateAnyValue(
      opentelemetry::proto::common::v1::AnyValue *proto_value,
      const opentelemetry::common::AttributeValue &value,
      AttributeValueMaxLength max_length = AttributeValueMaxLength{}) noexcept;

  static void PopulateAnyValue(
      opentelemetry::proto::common::v1::AnyValue *proto_value,
      const opentelemetry::sdk::common::OwnedAttributeValue &value,
      AttributeValueMaxLength max_length = AttributeValueMaxLength{}) noexcept;

  static void PopulateAttribute(
      opentelemetry::proto::common::v1::KeyValue *attribute,
      nostd::string_view key,
      const opentelemetry::common::AttributeValue &value,
      AttributeValueMaxLength max_length = AttributeValueMaxLength{}) noexcept;

  static void PopulateAttribute(
      opentelemetry::proto::common::v1::KeyValue *attribute,
      nostd::string_view key,
      const opentelemetry::sdk::common::OwnedAttributeValue &value,
      AttributeValueMaxLength max_length = AttributeValueMaxLength{}) noexcept;

  // Deprecated PopulateAnyValue and PopulateAttribute overloads. `allow_bytes` is no longer
  // meaningful because all AnyValue types support bytes. Prefer the AttributeValueMaxLength
  // overloads above.
  OPENTELEMETRY_DEPRECATED_MESSAGE(
      "allow_bytes is unused; use the AttributeValueMaxLength overload instead")
  static void PopulateAnyValue(opentelemetry::proto::common::v1::AnyValue *proto_value,
                               const opentelemetry::common::AttributeValue &value,
                               bool allow_bytes) noexcept;

  OPENTELEMETRY_DEPRECATED_MESSAGE(
      "allow_bytes is unused; use the AttributeValueMaxLength overload instead")
  static void PopulateAnyValue(opentelemetry::proto::common::v1::AnyValue *proto_value,
                               const opentelemetry::sdk::common::OwnedAttributeValue &value,
                               bool allow_bytes) noexcept;

  OPENTELEMETRY_DEPRECATED_MESSAGE(
      "allow_bytes is unused; use the AttributeValueMaxLength overload instead")
  static void PopulateAttribute(opentelemetry::proto::common::v1::KeyValue *attribute,
                                nostd::string_view key,
                                const opentelemetry::common::AttributeValue &value,
                                bool allow_bytes) noexcept;

  OPENTELEMETRY_DEPRECATED_MESSAGE(
      "allow_bytes is unused; use the AttributeValueMaxLength overload instead")
  static void PopulateAttribute(opentelemetry::proto::common::v1::KeyValue *attribute,
                                nostd::string_view key,
                                const opentelemetry::sdk::common::OwnedAttributeValue &value,
                                bool allow_bytes) noexcept;

  /**
   * Byte length of the longest prefix of `value` that fits within `max_bytes`
   * without splitting a well-formed UTF-8 multi-byte sequence. A lead byte's
   * declared length is only honored when its continuation bytes are present
   * and in range (0x80-0xBF); otherwise the lead is treated as a one-byte
   * unit, so malformed input degrades to plain byte truncation. The protobuf
   * `string` field type requires valid UTF-8, so this utility lets callers
   * truncate at a code-point boundary instead of cutting through a multi-byte
   * sequence.
   */
  static std::size_t Utf8SafePrefixLength(const char *data,
                                          std::size_t size,
                                          std::size_t max_bytes) noexcept;

  /// Convenience overload that delegates to the pointer + size variant.
  static std::size_t Utf8SafePrefixLength(const std::string &value, std::size_t max_bytes) noexcept
  {
    return Utf8SafePrefixLength(value.data(), value.size(), max_bytes);
  }
};

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
