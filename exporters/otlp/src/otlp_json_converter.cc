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

#include "opentelemetry/nostd/string_view.h"

// clang-format off
#include "opentelemetry/exporters/otlp/protobuf_include_prefix.h" // IWYU pragma: keep
// clang-format on
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <string_view>
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

// Hex-encodes into a stack buffer when the value fits, so trace and span IDs
// do not allocate a temporary string before the writer copies them.
void WriteHexString(JsonWriter &writer, const std::string &bytes)
{
  static constexpr std::size_t kStackBytes = 32;
  if (bytes.size() > kStackBytes)
  {
    writer.WriteString(HexEncode(bytes));
    return;
  }
  char buffer[kStackBytes * 2];
  for (std::size_t i = 0; i < bytes.size(); ++i)
  {
    unsigned char byte = static_cast<unsigned char>(bytes[i]);
    buffer[2 * i]      = HexEncode(static_cast<unsigned char>(byte >> 4));
    buffer[2 * i + 1]  = HexEncode(static_cast<unsigned char>(byte & 0x0f));
  }
  writer.WriteString(nostd::string_view(buffer, bytes.size() * 2));
}

// Formats right-aligned into the end of buffer and returns the first digit.
char *FormatDecimal(char *end, std::uint64_t value)
{
  do
  {
    *--end = static_cast<char>('0' + value % 10);
    value /= 10;
  } while (value != 0);
  return end;
}

// 64-bit integers are JSON strings in OTLP/JSON; formatting into a stack
// buffer avoids a temporary std::string that the writer would copy again.
void WriteUInt64String(JsonWriter &writer, std::uint64_t value)
{
  char buffer[20];
  char *end   = buffer + sizeof(buffer);
  char *begin = FormatDecimal(end, value);
  writer.WriteString(nostd::string_view(begin, static_cast<std::size_t>(end - begin)));
}

void WriteInt64String(JsonWriter &writer, std::int64_t value)
{
  char buffer[21];
  char *end = buffer + sizeof(buffer);
  // Negate as unsigned so INT64_MIN does not overflow.
  std::uint64_t magnitude =
      value < 0 ? 0 - static_cast<std::uint64_t>(value) : static_cast<std::uint64_t>(value);
  char *begin = FormatDecimal(end, magnitude);
  if (value < 0)
  {
    *--begin = '-';
  }
  writer.WriteString(nostd::string_view(begin, static_cast<std::size_t>(end - begin)));
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
        WriteHexString(writer, bytes);
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
      WriteHexString(writer, bytes);
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
      WriteInt64String(writer, message.GetReflection()->GetInt64(message, field_descriptor));
      break;
    }
    case google::protobuf::FieldDescriptor::CPPTYPE_UINT32: {
      writer.WriteUInt32(message.GetReflection()->GetUInt32(message, field_descriptor));
      break;
    }
    case google::protobuf::FieldDescriptor::CPPTYPE_UINT64: {
      // According to Protobuf specs 64-bit integer numbers in JSON-encoded payloads are encoded as
      // decimal strings, and either numbers or strings are accepted when decoding.
      WriteUInt64String(writer, message.GetReflection()->GetUInt64(message, field_descriptor));
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
        WriteInt64String(writer,
                         message.GetReflection()->GetRepeatedInt64(message, field_descriptor, i));
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
        WriteUInt64String(writer,
                          message.GetReflection()->GetRepeatedUInt64(message, field_descriptor, i));
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
// NOLINTEND(misc-no-recursion)

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
