// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>
#include <string>

#include "opentelemetry/common/attribute_value.h"
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
 * The OtlpCommoneUtils contains utility functions to populate attributes
 */
class OtlpPopulateAttributeUtils
{

public:
  static void PopulateAttribute(opentelemetry::proto::resource::v1::Resource *proto,
                                const opentelemetry::sdk::resource::Resource &resource) noexcept;

  static void PopulateAttribute(opentelemetry::proto::common::v1::InstrumentationScope *proto,
                                const opentelemetry::sdk::instrumentationscope::InstrumentationScope
                                    &instrumentation_scope) noexcept;

  static void PopulateAnyValue(opentelemetry::proto::common::v1::AnyValue *proto_value,
                               const opentelemetry::common::AttributeValue &value,
                               bool allow_bytes) noexcept;

  static void PopulateAnyValue(opentelemetry::proto::common::v1::AnyValue *proto_value,
                               const opentelemetry::sdk::common::OwnedAttributeValue &value,
                               bool allow_bytes) noexcept;

  static void PopulateAttribute(opentelemetry::proto::common::v1::KeyValue *attribute,
                                nostd::string_view key,
                                const opentelemetry::common::AttributeValue &value,
                                bool allow_bytes) noexcept;

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
  static std::size_t Utf8SafePrefixLength(const std::string &value, std::size_t max_bytes) noexcept;

  /**
   * Truncate the `string_value` and `bytes_value` (and recursively into
   * `array_value` elements) of an OTLP `AnyValue` to at most `max_length`
   * bytes each. `string_value` uses Utf8SafePrefixLength so the resulting
   * protobuf string stays valid UTF-8 when the input was. `bytes_value` is
   * cut at the raw byte boundary since it is not UTF-8 text.
   */
  static void TruncateProtoAttributeValue(opentelemetry::proto::common::v1::AnyValue *value,
                                          std::size_t max_length) noexcept;
};

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
