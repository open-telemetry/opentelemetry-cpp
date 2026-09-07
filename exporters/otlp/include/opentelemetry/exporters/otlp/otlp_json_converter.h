// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/exporters/otlp/otlp_http.h"
#include "opentelemetry/exporters/otlp/otlp_json_writer.h"
#include "opentelemetry/version.h"

// clang-format off
#include "opentelemetry/exporters/otlp/protobuf_include_prefix.h" // IWYU pragma: keep
#include <google/protobuf/message.h>
#include "opentelemetry/exporters/otlp/protobuf_include_suffix.h" // IWYU pragma: keep
// clang-format on

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

/**
 * Options that parameterize how a protobuf message is mapped to OTLP/JSON.
 */
struct JsonConverterOptions
{
  // If true, use the JSON name of a protobuf field to set the key of the JSON
  // object. If false, use the camel-case field name as generated from the
  // proto field name.
  bool use_json_name = false;

  // How `bytes` fields are mapped to JSON.
  JsonBytesMappingKind json_bytes_mapping = JsonBytesMappingKind::kHexId;
};

/**
 * Converts a protobuf message to OTLP/JSON using reflection, emitting through
 * `writer`.
 */
void ConvertGenericMessageToJson(JsonWriter &writer,
                                 const google::protobuf::Message &message,
                                 const JsonConverterOptions &options);

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
