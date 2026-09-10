// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/otlp/otlp_json_converter.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#if defined(HAVE_GSL)
#  include <gsl/gsl>
#else
#  include <cassert>
#endif

#include "opentelemetry/sdk/common/base64.h"

// clang-format off
#include "opentelemetry/exporters/otlp/protobuf_include_prefix.h" // IWYU pragma: keep
// clang-format on
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
// clang-format off
#include "opentelemetry/exporters/otlp/protobuf_include_suffix.h" // IWYU pragma: keep
// clang-format on

#ifdef GetMessage
#  undef GetMessage
#endif

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

namespace
{

inline char HexEncode(unsigned char byte)
{
#if defined(HAVE_GSL)
  Expects(byte <= 16);
#else
  assert(byte <= 16);
#endif
  if (byte >= 10)
  {
    return static_cast<char>(byte - 10 + 'a');
  }
  else
  {
    return static_cast<char>(byte + '0');
  }
}

std::string HexEncode(const std::string &bytes)
{
  std::string ret;
  ret.reserve(bytes.size() * 2);
  for (std::string::size_type i = 0; i < bytes.size(); ++i)
  {
    unsigned char byte = static_cast<unsigned char>(bytes[i]);
    ret.push_back(HexEncode(byte >> 4));
    ret.push_back(HexEncode(byte & 0x0f));
  }
  return ret;
}

void WriteBytesField(JsonWriter &writer,
                     const std::string &bytes,
                     const google::protobuf::FieldDescriptor *field_descriptor,
                     JsonBytesMappingKind kind)
{
  switch (kind)
  {
    case JsonBytesMappingKind::kHexId: {
      if (field_descriptor->lowercase_name() == "trace_id" ||
          field_descriptor->lowercase_name() == "span_id" ||
          field_descriptor->lowercase_name() == "parent_span_id")
      {
        writer.WriteString(HexEncode(bytes));
      }
      else
      {
        writer.WriteBytes(reinterpret_cast<const std::uint8_t *>(bytes.data()), bytes.size());
      }
      break;
    }
    case JsonBytesMappingKind::kBase64: {
      // Base64 is the default bytes mapping of protobuf
      writer.WriteBytes(reinterpret_cast<const std::uint8_t *>(bytes.data()), bytes.size());
      break;
    }
    case JsonBytesMappingKind::kHex:
      writer.WriteString(HexEncode(bytes));
      break;
    default:
      writer.WriteString(bytes);
      break;
  }
}

// NOLINTBEGIN(misc-no-recursion)
void ConvertGenericFieldToJson(JsonWriter &writer,
                               const google::protobuf::Message &message,
                               const google::protobuf::FieldDescriptor *field_descriptor,
                               const JsonConverterOptions &options)
{
  switch (field_descriptor->cpp_type())
  {
    case google::protobuf::FieldDescriptor::CPPTYPE_INT32: {
      writer.WriteInt32(message.GetReflection()->GetInt32(message, field_descriptor));
      break;
    }
    case google::protobuf::FieldDescriptor::CPPTYPE_INT64: {
      // According to Protobuf specs 64-bit integer numbers in JSON-encoded payloads are encoded as
      // decimal strings, and either numbers or strings are accepted when decoding.
      writer.WriteString(
          std::to_string(message.GetReflection()->GetInt64(message, field_descriptor)));
      break;
    }
    case google::protobuf::FieldDescriptor::CPPTYPE_UINT32: {
      writer.WriteUInt32(message.GetReflection()->GetUInt32(message, field_descriptor));
      break;
    }
    case google::protobuf::FieldDescriptor::CPPTYPE_UINT64: {
      // According to Protobuf specs 64-bit integer numbers in JSON-encoded payloads are encoded as
      // decimal strings, and either numbers or strings are accepted when decoding.
      writer.WriteString(
          std::to_string(message.GetReflection()->GetUInt64(message, field_descriptor)));
      break;
    }
    case google::protobuf::FieldDescriptor::CPPTYPE_STRING: {
      std::string empty;
      if (field_descriptor->type() == google::protobuf::FieldDescriptor::TYPE_BYTES)
      {
        WriteBytesField(
            writer, message.GetReflection()->GetStringReference(message, field_descriptor, &empty),
            field_descriptor, options.json_bytes_mapping);
      }
      else
      {
        writer.WriteString(
            message.GetReflection()->GetStringReference(message, field_descriptor, &empty));
      }
      break;
    }
    case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE: {
      ConvertGenericMessageToJson(
          writer, message.GetReflection()->GetMessage(message, field_descriptor, nullptr), options);
      break;
    }
    case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE: {
      writer.WriteDouble(message.GetReflection()->GetDouble(message, field_descriptor));
      break;
    }
    case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT: {
      writer.WriteDouble(message.GetReflection()->GetFloat(message, field_descriptor));
      break;
    }
    case google::protobuf::FieldDescriptor::CPPTYPE_BOOL: {
      writer.WriteBool(message.GetReflection()->GetBool(message, field_descriptor));
      break;
    }
    case google::protobuf::FieldDescriptor::CPPTYPE_ENUM: {
      writer.WriteInt32(message.GetReflection()->GetEnumValue(message, field_descriptor));
      break;
    }
    default: {
      writer.WriteNull();
      break;
    }
  }
}

void ConvertListFieldToJson(JsonWriter &writer,
                            const google::protobuf::Message &message,
                            const google::protobuf::FieldDescriptor *field_descriptor,
                            const JsonConverterOptions &options)
{
  auto field_size = message.GetReflection()->FieldSize(message, field_descriptor);

  writer.BeginArray();

  switch (field_descriptor->cpp_type())
  {
    case google::protobuf::FieldDescriptor::CPPTYPE_INT32: {
      for (int i = 0; i < field_size; ++i)
      {
        writer.WriteInt32(message.GetReflection()->GetRepeatedInt32(message, field_descriptor, i));
      }

      break;
    }
    case google::protobuf::FieldDescriptor::CPPTYPE_INT64: {
      for (int i = 0; i < field_size; ++i)
      {
        // According to Protobuf specs 64-bit integer numbers in JSON-encoded payloads are encoded
        // as decimal strings, and either numbers or strings are accepted when decoding.
        writer.WriteString(std::to_string(
            message.GetReflection()->GetRepeatedInt64(message, field_descriptor, i)));
      }

      break;
    }
    case google::protobuf::FieldDescriptor::CPPTYPE_UINT32: {
      for (int i = 0; i < field_size; ++i)
      {
        writer.WriteUInt32(
            message.GetReflection()->GetRepeatedUInt32(message, field_descriptor, i));
      }

      break;
    }
    case google::protobuf::FieldDescriptor::CPPTYPE_UINT64: {
      for (int i = 0; i < field_size; ++i)
      {
        // According to Protobuf specs 64-bit integer numbers in JSON-encoded payloads are encoded
        // as decimal strings, and either numbers or strings are accepted when decoding.
        writer.WriteString(std::to_string(
            message.GetReflection()->GetRepeatedUInt64(message, field_descriptor, i)));
      }

      break;
    }
    case google::protobuf::FieldDescriptor::CPPTYPE_STRING: {
      std::string empty;
      if (field_descriptor->type() == google::protobuf::FieldDescriptor::TYPE_BYTES)
      {
        for (int i = 0; i < field_size; ++i)
        {
          WriteBytesField(writer,
                          message.GetReflection()->GetRepeatedStringReference(
                              message, field_descriptor, i, &empty),
                          field_descriptor, options.json_bytes_mapping);
        }
      }
      else
      {
        for (int i = 0; i < field_size; ++i)
        {
          writer.WriteString(message.GetReflection()->GetRepeatedStringReference(
              message, field_descriptor, i, &empty));
        }
      }
      break;
    }
    case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE: {
      for (int i = 0; i < field_size; ++i)
      {
        ConvertGenericMessageToJson(
            writer, message.GetReflection()->GetRepeatedMessage(message, field_descriptor, i),
            options);
      }

      break;
    }
    case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE: {
      for (int i = 0; i < field_size; ++i)
      {
        writer.WriteDouble(
            message.GetReflection()->GetRepeatedDouble(message, field_descriptor, i));
      }

      break;
    }
    case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT: {
      for (int i = 0; i < field_size; ++i)
      {
        writer.WriteDouble(message.GetReflection()->GetRepeatedFloat(message, field_descriptor, i));
      }

      break;
    }
    case google::protobuf::FieldDescriptor::CPPTYPE_BOOL: {
      for (int i = 0; i < field_size; ++i)
      {
        writer.WriteBool(message.GetReflection()->GetRepeatedBool(message, field_descriptor, i));
      }

      break;
    }
    case google::protobuf::FieldDescriptor::CPPTYPE_ENUM: {
      for (int i = 0; i < field_size; ++i)
      {
        writer.WriteInt32(
            message.GetReflection()->GetRepeatedEnumValue(message, field_descriptor, i));
      }
      break;
    }
    default: {
      break;
    }
  }

  writer.EndArray();
}

}  // namespace
// NOLINTEND(misc-no-recursion)

void ConvertGenericMessageToJson(JsonWriter &writer,
                                 const google::protobuf::Message &message,
                                 const JsonConverterOptions &options)
{
  std::vector<const google::protobuf::FieldDescriptor *> fields_with_data;
  message.GetReflection()->ListFields(message, &fields_with_data);

  // A message with no set fields serializes as null, not as an empty object.
  if (fields_with_data.empty())
  {
    writer.WriteNull();
    return;
  }

  writer.BeginObject();
  for (std::size_t i = 0; i < fields_with_data.size(); ++i)
  {
    const google::protobuf::FieldDescriptor *field_descriptor = fields_with_data[i];
    const auto &field_name =
        options.use_json_name ? field_descriptor->json_name() : field_descriptor->camelcase_name();
    writer.Key(nostd::string_view(field_name.data(), field_name.size()));
    if (field_descriptor->is_repeated())
    {
      ConvertListFieldToJson(writer, message, field_descriptor, options);
    }
    else
    {
      ConvertGenericFieldToJson(writer, message, field_descriptor, options);
    }
  }
  writer.EndObject();
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
