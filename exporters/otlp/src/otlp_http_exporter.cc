// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/otlp/otlp_http_exporter.h"
#include "opentelemetry/exporters/otlp/otlp_recordable.h"
#include "opentelemetry/ext/http/client/http_client_factory.h"
#include "opentelemetry/ext/http/common/url_parser.h"

#include "nlohmann/json.hpp"

#include "opentelemetry/exporters/otlp/protobuf_include_prefix.h"

#include "google/protobuf/message.h"
#include "google/protobuf/reflection.h"
#include "google/protobuf/stubs/stl_util.h"

#include "opentelemetry/proto/collector/trace/v1/trace_service.pb.h"

#include "opentelemetry/exporters/otlp/protobuf_include_suffix.h"

#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk_config.h"

#include <condition_variable>
#include <fstream>
#include <mutex>
#include <sstream>
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

/**
 * This class handles the response message from the Elasticsearch request
 */
class ResponseHandler : public http_client::EventHandler
{
public:
  /**
   * Creates a response handler, that by default doesn't display to console
   */
  ResponseHandler(bool console_debug = false) : console_debug_{console_debug} {}

  /**
   * Automatically called when the response is received, store the body into a string and notify any
   * threads blocked on this result
   */
  void OnResponse(http_client::Response &response) noexcept override
  {
    // Lock the private members so they can't be read while being modified
    {
      std::unique_lock<std::mutex> lk(mutex_);

      // Store the body of the request
      body_ = std::string(response.GetBody().begin(), response.GetBody().end());

      if (console_debug_)
      {
        std::stringstream ss;
        ss << "[OTLP HTTP Exporter] Status:" << response.GetStatusCode() << "Header:";
        response.ForEachHeader([&ss](opentelemetry::nostd::string_view header_name,
                                     opentelemetry::nostd::string_view header_value) {
          ss << "\t" << header_name.data() << " : " << header_value.data() << ",";
          return true;
        });
        ss << "Body:" << body_;
        OTEL_INTERNAL_LOG_DEBUG(ss.str());
      }

      // Set the response_received_ flag to true and notify any threads waiting on this result
      response_received_ = true;
    }
    cv_.notify_all();
  }

  /**resource
   * A method the user calls to block their thread until the response is received. The longest
   * duration is the timeout of the request, set by SetTimeoutMs()
   */
  bool waitForResponse()
  {
    std::unique_lock<std::mutex> lk(mutex_);
    cv_.wait(lk);
    return response_received_;
  }

  /**
   * Returns the body of the response
   */
  std::string GetResponseBody()
  {
    // Lock so that body_ can't be written to while returning it
    std::unique_lock<std::mutex> lk(mutex_);
    return body_;
  }

  // Callback method when an http event occurs
  void OnEvent(http_client::SessionState state,
               opentelemetry::nostd::string_view reason) noexcept override
  {
    // If any failure event occurs, release the condition variable to unblock main thread
    switch (state)
    {
      case http_client::SessionState::CreateFailed:
        OTEL_INTERNAL_LOG_ERROR("[OTLP HTTP Exporter] Session state: session create failed");
        cv_.notify_all();
        break;

      case http_client::SessionState::Created:
        if (console_debug_)
        {
          OTEL_INTERNAL_LOG_DEBUG("[OTLP HTTP Exporter] Session state: session created");
        }
        break;

      case http_client::SessionState::Destroyed:
        if (console_debug_)
        {
          OTEL_INTERNAL_LOG_DEBUG("[OTLP HTTP Exporter] Session state: session destroyed");
        }
        break;

      case http_client::SessionState::Connecting:
        if (console_debug_)
        {
          OTEL_INTERNAL_LOG_DEBUG("[OTLP HTTP Exporter] Session state: connecting to peer");
        }
        break;

      case http_client::SessionState::ConnectFailed:
        OTEL_INTERNAL_LOG_ERROR("[OTLP HTTP Exporter] Session state: connection failed");
        cv_.notify_all();
        break;

      case http_client::SessionState::Connected:
        if (console_debug_)
        {
          OTEL_INTERNAL_LOG_DEBUG("[OTLP HTTP Exporter] Session state: connected");
        }
        break;

      case http_client::SessionState::Sending:
        if (console_debug_)
        {
          OTEL_INTERNAL_LOG_DEBUG("[OTLP HTTP Exporter] Session state: sending request");
        }
        break;

      case http_client::SessionState::SendFailed:
        OTEL_INTERNAL_LOG_ERROR("[OTLP HTTP Exporter] Session state: request send failed");
        cv_.notify_all();
        break;

      case http_client::SessionState::Response:
        if (console_debug_)
        {
          OTEL_INTERNAL_LOG_DEBUG("[OTLP HTTP Exporter] Session state: response received");
        }
        break;

      case http_client::SessionState::SSLHandshakeFailed:
        OTEL_INTERNAL_LOG_ERROR("[OTLP HTTP Exporter] Session state: SSL handshake failed");
        cv_.notify_all();
        break;

      case http_client::SessionState::TimedOut:
        OTEL_INTERNAL_LOG_ERROR("[OTLP HTTP Exporter] Session state: request time out");
        cv_.notify_all();
        break;

      case http_client::SessionState::NetworkError:
        OTEL_INTERNAL_LOG_ERROR("[OTLP HTTP Exporter] Session state: network error");
        cv_.notify_all();
        break;

      case http_client::SessionState::ReadError:
        if (console_debug_)
        {
          OTEL_INTERNAL_LOG_DEBUG("[OTLP HTTP Exporter] Session state: error reading response");
        }
        break;

      case http_client::SessionState::WriteError:
        if (console_debug_)
        {
          OTEL_INTERNAL_LOG_DEBUG(
              "[OTLP HTTP Exporter] DEBUG:Session state: error writing request");
        }
        break;

      case http_client::SessionState::Cancelled:
        OTEL_INTERNAL_LOG_ERROR("[OTLP HTTP Exporter] Session state: (manually) cancelled\n");
        cv_.notify_all();
        break;

      default:
        break;
    }
  }

private:
  // Define a condition variable and mutex
  std::condition_variable cv_;
  std::mutex mutex_;

  // Whether the response from Elasticsearch has been received
  bool response_received_ = false;

  // A string to store the response body
  std::string body_ = "";

  // Whether to print the results from the callback
  bool console_debug_ = false;
};

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

OtlpHttpExporter::OtlpHttpExporter(const OtlpHttpExporterOptions &options)
    : options_(options), http_client_(http_client::HttpClientFactory::Create())
{}

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
    const char *error_message = "[OTLP HTTP Exporter] Export failed, exporter is shutdown";
    if (options_.console_debug)
    {
      std::cerr << error_message << std::endl;
    }
    OTEL_INTERNAL_LOG_ERROR(error_message);

    return sdk::common::ExportResult::kFailure;
  }

  // Parse uri and store it to cache
  if (http_uri_.empty())
  {
    auto parse_url = opentelemetry::ext::http::common::UrlParser(std::string(options_.url));
    if (!parse_url.success_)
    {
      std::string error_message =
          "[OTLP HTTP Exporter] Export failed, invalid url: " + options_.url;
      if (options_.console_debug)
      {
        std::cerr << error_message << std::endl;
      }
      OTEL_INTERNAL_LOG_ERROR(error_message.c_str());

      return sdk::common::ExportResult::kFailure;
    }

    if (!parse_url.path_.empty() && parse_url.path_[0] == '/')
    {
      http_uri_ = parse_url.path_.substr(1);
    }
    else
    {
      http_uri_ = parse_url.path_;
    }
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
        OTEL_INTERNAL_LOG_DEBUG(
            "[OTLP HTTP Exporter] Request body(Binary): " << service_request.Utf8DebugString());
      }
    }
    else
    {
      if (options_.console_debug)
      {
        OTEL_INTERNAL_LOG_DEBUG("[OTLP HTTP Exporter] Serialize body failed(Binary):"
                                << service_request.InitializationErrorString());
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
      OTEL_INTERNAL_LOG_DEBUG("[OTLP HTTP Exporter] Request body(Json)" << post_body_json);
    }
    body_vec.assign(post_body_json.begin(), post_body_json.end());
    content_type = kHttpJsonContentType;
  }

  // Send the request
  auto session = http_client_->CreateSession(options_.url);
  auto request = session->CreateRequest();

  for (auto &header : options_.http_headers)
  {
    request->AddHeader(header.first, header.second);
  }
  request->SetUri(http_uri_);
  request->SetTimeoutMs(std::chrono::duration_cast<std::chrono::milliseconds>(options_.timeout));
  request->SetMethod(http_client::Method::Post);
  request->SetBody(body_vec);
  request->ReplaceHeader("Content-Type", content_type);

  // Send the request
  std::unique_ptr<ResponseHandler> handler(new ResponseHandler(options_.console_debug));
  session->SendRequest(*handler);

  // Wait for the response to be received
  if (options_.console_debug)
  {
    OTEL_INTERNAL_LOG_DEBUG(
        "[OTLP HTTP Exporter] DEBUG: Waiting for response from "
        << options_.url << " (timeout = "
        << std::chrono::duration_cast<std::chrono::milliseconds>(options_.timeout).count()
        << " milliseconds)");
  }
  bool write_successful = handler->waitForResponse();

  // End the session
  session->FinishSession();

  // If an error occurred with the HTTP request
  if (!write_successful)
  {
    // TODO: retry logic
    return sdk::common::ExportResult::kFailure;
  }

  return sdk::common::ExportResult::kSuccess;
}

bool OtlpHttpExporter::Shutdown(std::chrono::microseconds) noexcept
{
  is_shutdown_ = true;

  // Shutdown the session manager
  http_client_->CancelAllSessions();
  http_client_->FinishAllSessions();

  return true;
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
