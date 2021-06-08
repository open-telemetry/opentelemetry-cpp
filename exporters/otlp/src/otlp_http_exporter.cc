// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/otlp/otlp_http_exporter.h"
#include "opentelemetry/exporters/otlp/otlp_recordable.h"
#include "opentelemetry/ext/http/client/http_client_factory.h"

#include "nlohmann/json.hpp"

#include "opentelemetry/exporters/otlp/protobuf_include_prefix.h"

#include "google/protobuf/message.h"
#include "google/protobuf/reflection.h"
#include "google/protobuf/stubs/stl_util.h"
#include "opentelemetry/proto/collector/trace/v1/trace_service.pb.h"

#include "opentelemetry/exporters/otlp/protobuf_include_suffix.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#ifdef GetMessage
#  undef GetMessage
#endif

namespace nostd       = opentelemetry::nostd;
namespace http_client = opentelemetry::ext::http::client;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

namespace
{

static inline char HexEncode(unsigned char byte)
{
  if (byte >= 10)
  {
    return byte - 10 + 'a';
  }
  else
  {
    return byte + '0';
  }
}

static std::string HexEncode(const std::string &bytes)
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

static std::string BytesMapping(const std::string &bytes,
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
        return HexEncode(bytes);
      }
      else
      {
        std::string base64_value;
        google::protobuf::Base64Escape(bytes, &base64_value);
        return base64_value;
      }
    }
    case JsonBytesMappingKind::kBase64: {
      // Base64 is the default bytes mapping of protobuf
      std::string base64_value;
      google::protobuf::Base64Escape(bytes, &base64_value);
      return base64_value;
    }
    case JsonBytesMappingKind::kHex:
      return HexEncode(bytes);
    default:
      return bytes;
  }
}

static void ConvertGenericFieldToJson(nlohmann::json &value,
                                      const google::protobuf::Message &message,
                                      const google::protobuf::FieldDescriptor *field_descriptor,
                                      const OtlpHttpExporterOptions &options);

static void ConvertListFieldToJson(nlohmann::json &value,
                                   const google::protobuf::Message &message,
                                   const google::protobuf::FieldDescriptor *field_descriptor,
                                   const OtlpHttpExporterOptions &options);

static void ConvertGenericMessageToJson(nlohmann::json &value,
                                        const google::protobuf::Message &message,
                                        const OtlpHttpExporterOptions &options)
{
  std::vector<const google::protobuf::FieldDescriptor *> fields_with_data;
  message.GetReflection()->ListFields(message, &fields_with_data);
  for (std::size_t i = 0; i < fields_with_data.size(); ++i)
  {
    const google::protobuf::FieldDescriptor *field_descriptor = fields_with_data[i];
    nlohmann::json &child_value = options.use_json_name ? value[field_descriptor->json_name()]
                                                        : value[field_descriptor->name()];
    if (field_descriptor->is_repeated())
    {
      ConvertListFieldToJson(child_value, message, field_descriptor, options);
    }
    else
    {
      ConvertGenericFieldToJson(child_value, message, field_descriptor, options);
    }
  }
}

void ConvertGenericFieldToJson(nlohmann::json &value,
                               const google::protobuf::Message &message,
                               const google::protobuf::FieldDescriptor *field_descriptor,
                               const OtlpHttpExporterOptions &options)
{
  switch (field_descriptor->cpp_type())
  {
    case google::protobuf::FieldDescriptor::CPPTYPE_INT32: {
      value = message.GetReflection()->GetInt32(message, field_descriptor);
      break;
    }
    case google::protobuf::FieldDescriptor::CPPTYPE_INT64: {
      // According to Protobuf specs 64-bit integer numbers in JSON-encoded payloads are encoded as
      // decimal strings, and either numbers or strings are accepted when decoding.
      value = std::to_string(message.GetReflection()->GetInt64(message, field_descriptor));
      break;
    }
    case google::protobuf::FieldDescriptor::CPPTYPE_UINT32: {
      value = message.GetReflection()->GetUInt32(message, field_descriptor);
      break;
    }
    case google::protobuf::FieldDescriptor::CPPTYPE_UINT64: {
      // According to Protobuf specs 64-bit integer numbers in JSON-encoded payloads are encoded as
      // decimal strings, and either numbers or strings are accepted when decoding.
      value = std::to_string(message.GetReflection()->GetUInt64(message, field_descriptor));
      break;
    }
    case google::protobuf::FieldDescriptor::CPPTYPE_STRING: {
      std::string empty;
      if (field_descriptor->type() == google::protobuf::FieldDescriptor::TYPE_BYTES)
      {
        value = BytesMapping(
            message.GetReflection()->GetStringReference(message, field_descriptor, &empty),
            field_descriptor, options.json_bytes_mapping);
      }
      else
      {
        value = message.GetReflection()->GetStringReference(message, field_descriptor, &empty);
      }
      break;
    }
    case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE: {
      ConvertGenericMessageToJson(
          value, message.GetReflection()->GetMessage(message, field_descriptor, nullptr), options);
      break;
    }
    case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE: {
      value = message.GetReflection()->GetDouble(message, field_descriptor);
      break;
    }
    case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT: {
      value = message.GetReflection()->GetFloat(message, field_descriptor);
      break;
    }
    case google::protobuf::FieldDescriptor::CPPTYPE_BOOL: {
      value = message.GetReflection()->GetBool(message, field_descriptor);
      break;
    }
    case google::protobuf::FieldDescriptor::CPPTYPE_ENUM: {
      value = message.GetReflection()->GetEnumValue(message, field_descriptor);
      break;
    }
    default: {
      break;
    }
  }
}

void ConvertListFieldToJson(nlohmann::json &value,
                            const google::protobuf::Message &message,
                            const google::protobuf::FieldDescriptor *field_descriptor,
                            const OtlpHttpExporterOptions &options)
{
  auto field_size = message.GetReflection()->FieldSize(message, field_descriptor);

  switch (field_descriptor->cpp_type())
  {
    case google::protobuf::FieldDescriptor::CPPTYPE_INT32: {
      for (int i = 0; i < field_size; ++i)
      {
        value.push_back(message.GetReflection()->GetRepeatedInt32(message, field_descriptor, i));
      }

      break;
    }
    case google::protobuf::FieldDescriptor::CPPTYPE_INT64: {
      for (int i = 0; i < field_size; ++i)
      {
        // According to Protobuf specs 64-bit integer numbers in JSON-encoded payloads are encoded
        // as decimal strings, and either numbers or strings are accepted when decoding.
        value.push_back(std::to_string(
            message.GetReflection()->GetRepeatedInt64(message, field_descriptor, i)));
      }

      break;
    }
    case google::protobuf::FieldDescriptor::CPPTYPE_UINT32: {
      for (int i = 0; i < field_size; ++i)
      {
        value.push_back(message.GetReflection()->GetRepeatedUInt32(message, field_descriptor, i));
      }

      break;
    }
    case google::protobuf::FieldDescriptor::CPPTYPE_UINT64: {
      for (int i = 0; i < field_size; ++i)
      {
        // According to Protobuf specs 64-bit integer numbers in JSON-encoded payloads are encoded
        // as decimal strings, and either numbers or strings are accepted when decoding.
        value.push_back(std::to_string(
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
          value.push_back(BytesMapping(message.GetReflection()->GetRepeatedStringReference(
                                           message, field_descriptor, i, &empty),
                                       field_descriptor, options.json_bytes_mapping));
        }
      }
      else
      {
        for (int i = 0; i < field_size; ++i)
        {
          value.push_back(message.GetReflection()->GetRepeatedStringReference(
              message, field_descriptor, i, &empty));
        }
      }
      break;
    }
    case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE: {
      for (int i = 0; i < field_size; ++i)
      {
        nlohmann::json sub_value;
        ConvertGenericMessageToJson(
            sub_value, message.GetReflection()->GetRepeatedMessage(message, field_descriptor, i),
            options);
        value.push_back(std::move(sub_value));
      }

      break;
    }
    case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE: {
      for (int i = 0; i < field_size; ++i)
      {
        value.push_back(message.GetReflection()->GetRepeatedDouble(message, field_descriptor, i));
      }

      break;
    }
    case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT: {
      for (int i = 0; i < field_size; ++i)
      {
        value.push_back(message.GetReflection()->GetRepeatedFloat(message, field_descriptor, i));
      }

      break;
    }
    case google::protobuf::FieldDescriptor::CPPTYPE_BOOL: {
      for (int i = 0; i < field_size; ++i)
      {
        value.push_back(message.GetReflection()->GetRepeatedBool(message, field_descriptor, i));
      }

      break;
    }
    case google::protobuf::FieldDescriptor::CPPTYPE_ENUM: {
      for (int i = 0; i < field_size; ++i)
      {
        value.push_back(
            message.GetReflection()->GetRepeatedEnumValue(message, field_descriptor, i));
      }
      break;
    }
    default: {
      break;
    }
  }
}

/**
 * Add span protobufs contained in recordables to request.
 * @param spans the spans to export
 * @param request the current request
 */
static void PopulateRequest(
    const nostd::span<std::unique_ptr<sdk::trace::Recordable>> &spans,
    opentelemetry::proto::collector::trace::v1::ExportTraceServiceRequest *request)
{
  auto resource_span       = request->add_resource_spans();
  auto instrumentation_lib = resource_span->add_instrumentation_library_spans();
  bool has_resource        = false;

  for (auto &recordable : spans)
  {
    auto rec = std::unique_ptr<OtlpRecordable>(
        static_cast<exporter::otlp::OtlpRecordable *>(recordable.release()));
    *instrumentation_lib->add_spans() = std::move(rec->span());

    if (!has_resource)
    {
      *resource_span->mutable_resource() = rec->ProtoResource();
      has_resource                       = true;
    }
  }
}

}  // namespace

OtlpHttpExporter::OtlpHttpExporter() : OtlpHttpExporter(OtlpHttpExporterOptions()) {}

OtlpHttpExporter::OtlpHttpExporter(const OtlpHttpExporterOptions &options) : options_(options) {}

// ----------------------------- Exporter methods ------------------------------

std::unique_ptr<sdk::trace::Recordable> OtlpHttpExporter::MakeRecordable() noexcept
{
  return std::unique_ptr<sdk::trace::Recordable>(new exporter::otlp::OtlpRecordable());
}

sdk::common::ExportResult OtlpHttpExporter::Export(
    const nostd::span<std::unique_ptr<sdk::trace::Recordable>> &spans) noexcept
{
  // Return failure if this exporter has been shutdown
  if (is_shutdown_)
  {
    if (options_.console_debug)
    {
      std::cerr << "[OTLP HTTP Exporter] Export failed, exporter is shutdown" << std::endl;
    }

    return sdk::common::ExportResult::kFailure;
  }

  proto::collector::trace::v1::ExportTraceServiceRequest service_request;
  PopulateRequest(spans, &service_request);

  http_client::Body body_vec;
  std::string content_type;
  if (options_.content_type == HttpRequestContentType::kBinary)
  {
    body_vec.resize(service_request.ByteSizeLong());
    if (service_request.SerializeWithCachedSizesToArray(
            reinterpret_cast<google::protobuf::uint8 *>(&body_vec[0])))
    {
      if (options_.console_debug)
      {
        std::cout << "[OTLP HTTP Exporter] Request body(Binary):\n"
                  << service_request.Utf8DebugString() << std::endl;
      }
    }
    else
    {
      if (options_.console_debug)
      {
        std::cout << "[OTLP HTTP Exporter] Serialize body failed(Binary):"
                  << service_request.InitializationErrorString() << std::endl;
      }
      return sdk::common::ExportResult::kFailure;
    }
    content_type = kHttpBinaryContentType;
  }
  else
  {
    nlohmann::json json_request;

    // Convert from proto into json object
    ConvertGenericMessageToJson(json_request, service_request, options_);

    std::string post_body_json =
        json_request.dump(-1, ' ', false, nlohmann::detail::error_handler_t::replace);
    if (options_.console_debug)
    {
      std::cout << "[OTLP HTTP Exporter] Request body(Json):\n" << post_body_json << std::endl;
    }
    body_vec.assign(post_body_json.begin(), post_body_json.end());
    content_type = kHttpJsonContentType;
  }

  // Send the request
  auto client = http_client::HttpClientFactory::CreateSync();
  // TODO: Set timeout
  auto result = client->Post(options_.url, body_vec, {{"Content-Type", content_type}});

  // If an error occurred with the HTTP request
  if (!result)
  {
    if (options_.console_debug)
    {
      switch (result.GetSessionState())
      {
        case http_client::SessionState::CreateFailed:
          std::cerr << "[OTLP HTTP Exporter] session state: session create failed" << std::endl;
          break;

        case http_client::SessionState::Created:
          std::cerr << "[OTLP HTTP Exporter] session state: session created" << std::endl;
          break;

        case http_client::SessionState::Destroyed:
          std::cerr << "[OTLP HTTP Exporter] session state: session destroyed" << std::endl;
          break;

        case http_client::SessionState::Connecting:
          std::cerr << "[OTLP HTTP Exporter] session state: connecting to peer" << std::endl;
          break;

        case http_client::SessionState::ConnectFailed:
          std::cerr << "[OTLP HTTP Exporter] session state: connection failed" << std::endl;
          break;

        case http_client::SessionState::Connected:
          std::cerr << "[OTLP HTTP Exporter] session state: connected" << std::endl;
          break;

        case http_client::SessionState::Sending:
          std::cerr << "[OTLP HTTP Exporter] session state: sending request" << std::endl;
          break;

        case http_client::SessionState::SendFailed:
          std::cerr << "[OTLP HTTP Exporter] session state: request send failed" << std::endl;
          break;

        case http_client::SessionState::Response:
          std::cerr << "[OTLP HTTP Exporter] session state: response received" << std::endl;
          break;

        case http_client::SessionState::SSLHandshakeFailed:
          std::cerr << "[OTLP HTTP Exporter] session state: SSL handshake failed" << std::endl;
          break;

        case http_client::SessionState::TimedOut:
          std::cerr << "[OTLP HTTP Exporter] session state: request time out" << std::endl;
          break;

        case http_client::SessionState::NetworkError:
          std::cerr << "[OTLP HTTP Exporter] session state: network error" << std::endl;
          break;

        case http_client::SessionState::ReadError:
          std::cerr << "[OTLP HTTP Exporter] session state: error reading response" << std::endl;
          break;

        case http_client::SessionState::WriteError:
          std::cerr << "[OTLP HTTP Exporter] session state: error writing request" << std::endl;
          break;

        case http_client::SessionState::Cancelled:
          std::cerr << "[OTLP HTTP Exporter] session state: (manually) cancelled" << std::endl;
          break;

        default:
          break;
      }
    }
    // TODO: retry logic
    return sdk::common::ExportResult::kFailure;
  }

  if (options_.console_debug)
  {
    std::cout << "[OTLP HTTP Exporter] Status:" << result.GetResponse().GetStatusCode() << std::endl
              << "Header:" << std::endl;
    result.GetResponse().ForEachHeader([](opentelemetry::nostd::string_view header_name,
                                          opentelemetry::nostd::string_view header_value) {
      std::cout << "\t" << header_name.data() << " : " << header_value.data() << std::endl;
      return true;
    });
    std::cout << "Body:" << std::endl
              << std::string(result.GetResponse().GetBody().begin(),
                             result.GetResponse().GetBody().end())
              << std::endl;
  }

  return sdk::common::ExportResult::kSuccess;
}

bool OtlpHttpExporter::Shutdown(std::chrono::microseconds) noexcept
{
  is_shutdown_ = true;

  // TODO: Shutdown the http request

  return true;
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
