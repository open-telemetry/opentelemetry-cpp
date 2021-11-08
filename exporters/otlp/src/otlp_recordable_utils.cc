// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/otlp/otlp_recordable_utils.h"

#include "opentelemetry/exporters/otlp/protobuf_include_prefix.h"

#include "opentelemetry/proto/logs/v1/logs.pb.h"
#include "opentelemetry/proto/trace/v1/trace.pb.h"

#include "opentelemetry/exporters/otlp/protobuf_include_suffix.h"

#include "opentelemetry/exporters/otlp/otlp_log_recordable.h"
#include "opentelemetry/exporters/otlp/otlp_recordable.h"

namespace nostd = opentelemetry::nostd;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

//
// See `attribute_value.h` for details.
//
const int kAttributeValueSize      = 16;
const int kOwnedAttributeValueSize = 15;

void OtlpRecordableUtils::PopulateAttribute(
    opentelemetry::proto::common::v1::KeyValue *attribute,
    nostd::string_view key,
    const opentelemetry::common::AttributeValue &value) noexcept
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

  if (nostd::holds_alternative<bool>(value))
  {
    attribute->mutable_value()->set_bool_value(nostd::get<bool>(value));
  }
  else if (nostd::holds_alternative<int>(value))
  {
    attribute->mutable_value()->set_int_value(nostd::get<int>(value));
  }
  else if (nostd::holds_alternative<int64_t>(value))
  {
    attribute->mutable_value()->set_int_value(nostd::get<int64_t>(value));
  }
  else if (nostd::holds_alternative<unsigned int>(value))
  {
    attribute->mutable_value()->set_int_value(nostd::get<unsigned int>(value));
  }
  else if (nostd::holds_alternative<uint64_t>(value))
  {
    attribute->mutable_value()->set_int_value(nostd::get<uint64_t>(value));
  }
  else if (nostd::holds_alternative<double>(value))
  {
    attribute->mutable_value()->set_double_value(nostd::get<double>(value));
  }
  else if (nostd::holds_alternative<const char *>(value))
  {
    attribute->mutable_value()->set_string_value(nostd::get<const char *>(value));
  }
  else if (nostd::holds_alternative<nostd::string_view>(value))
  {
    attribute->mutable_value()->set_string_value(nostd::get<nostd::string_view>(value).data(),
                                                 nostd::get<nostd::string_view>(value).size());
  }
  else if (nostd::holds_alternative<nostd::span<const uint8_t>>(value))
  {
    for (const auto &val : nostd::get<nostd::span<const uint8_t>>(value))
    {
      attribute->mutable_value()->mutable_array_value()->add_values()->set_int_value(val);
    }
  }
  else if (nostd::holds_alternative<nostd::span<const bool>>(value))
  {
    for (const auto &val : nostd::get<nostd::span<const bool>>(value))
    {
      attribute->mutable_value()->mutable_array_value()->add_values()->set_bool_value(val);
    }
  }
  else if (nostd::holds_alternative<nostd::span<const int>>(value))
  {
    for (const auto &val : nostd::get<nostd::span<const int>>(value))
    {
      attribute->mutable_value()->mutable_array_value()->add_values()->set_int_value(val);
    }
  }
  else if (nostd::holds_alternative<nostd::span<const int64_t>>(value))
  {
    for (const auto &val : nostd::get<nostd::span<const int64_t>>(value))
    {
      attribute->mutable_value()->mutable_array_value()->add_values()->set_int_value(val);
    }
  }
  else if (nostd::holds_alternative<nostd::span<const unsigned int>>(value))
  {
    for (const auto &val : nostd::get<nostd::span<const unsigned int>>(value))
    {
      attribute->mutable_value()->mutable_array_value()->add_values()->set_int_value(val);
    }
  }
  else if (nostd::holds_alternative<nostd::span<const uint64_t>>(value))
  {
    for (const auto &val : nostd::get<nostd::span<const uint64_t>>(value))
    {
      attribute->mutable_value()->mutable_array_value()->add_values()->set_int_value(val);
    }
  }
  else if (nostd::holds_alternative<nostd::span<const double>>(value))
  {
    for (const auto &val : nostd::get<nostd::span<const double>>(value))
    {
      attribute->mutable_value()->mutable_array_value()->add_values()->set_double_value(val);
    }
  }
  else if (nostd::holds_alternative<nostd::span<const nostd::string_view>>(value))
  {
    for (const auto &val : nostd::get<nostd::span<const nostd::string_view>>(value))
    {
      attribute->mutable_value()->mutable_array_value()->add_values()->set_string_value(val.data(),
                                                                                        val.size());
    }
  }
}

/** Maps from C++ attribute into OTLP proto attribute. */
void OtlpRecordableUtils::PopulateAttribute(
    opentelemetry::proto::common::v1::KeyValue *attribute,
    nostd::string_view key,
    const opentelemetry::sdk::common::OwnedAttributeValue &value) noexcept
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

  if (nostd::holds_alternative<bool>(value))
  {
    attribute->mutable_value()->set_bool_value(nostd::get<bool>(value));
  }
  else if (nostd::holds_alternative<int32_t>(value))
  {
    attribute->mutable_value()->set_int_value(nostd::get<int32_t>(value));
  }
  else if (nostd::holds_alternative<int64_t>(value))
  {
    attribute->mutable_value()->set_int_value(nostd::get<int64_t>(value));
  }
  else if (nostd::holds_alternative<uint32_t>(value))
  {
    attribute->mutable_value()->set_int_value(nostd::get<uint32_t>(value));
  }
  else if (nostd::holds_alternative<uint64_t>(value))
  {
    attribute->mutable_value()->set_int_value(nostd::get<uint64_t>(value));
  }
  else if (nostd::holds_alternative<double>(value))
  {
    attribute->mutable_value()->set_double_value(nostd::get<double>(value));
  }
  else if (nostd::holds_alternative<std::string>(value))
  {
    attribute->mutable_value()->set_string_value(nostd::get<std::string>(value));
  }
  else if (nostd::holds_alternative<std::vector<bool>>(value))
  {
    for (const auto &val : nostd::get<std::vector<bool>>(value))
    {
      attribute->mutable_value()->mutable_array_value()->add_values()->set_bool_value(val);
    }
  }
  else if (nostd::holds_alternative<std::vector<int32_t>>(value))
  {
    for (const auto &val : nostd::get<std::vector<int32_t>>(value))
    {
      attribute->mutable_value()->mutable_array_value()->add_values()->set_int_value(val);
    }
  }
  else if (nostd::holds_alternative<std::vector<uint32_t>>(value))
  {
    for (const auto &val : nostd::get<std::vector<uint32_t>>(value))
    {
      attribute->mutable_value()->mutable_array_value()->add_values()->set_int_value(val);
    }
  }
  else if (nostd::holds_alternative<std::vector<int64_t>>(value))
  {
    for (const auto &val : nostd::get<std::vector<int64_t>>(value))
    {
      attribute->mutable_value()->mutable_array_value()->add_values()->set_int_value(val);
    }
  }
  else if (nostd::holds_alternative<std::vector<uint64_t>>(value))
  {
    for (const auto &val : nostd::get<std::vector<uint64_t>>(value))
    {
      attribute->mutable_value()->mutable_array_value()->add_values()->set_int_value(val);
    }
  }
  else if (nostd::holds_alternative<std::vector<double>>(value))
  {
    for (const auto &val : nostd::get<std::vector<double>>(value))
    {
      attribute->mutable_value()->mutable_array_value()->add_values()->set_double_value(val);
    }
  }
  else if (nostd::holds_alternative<std::vector<std::string>>(value))
  {
    for (const auto &val : nostd::get<std::vector<std::string>>(value))
    {
      attribute->mutable_value()->mutable_array_value()->add_values()->set_string_value(val);
    }
  }
}

void OtlpRecordableUtils::PopulateAttribute(
    opentelemetry::proto::resource::v1::Resource *proto,
    const opentelemetry::sdk::resource::Resource &resource) noexcept
{
  if (nullptr == proto)
  {
    return;
  }

  for (const auto &kv : resource.GetAttributes())
  {
    OtlpRecordableUtils::PopulateAttribute(proto->add_attributes(), kv.first, kv.second);
  }
}

void OtlpRecordableUtils::PopulateRequest(
    const nostd::span<std::unique_ptr<opentelemetry::sdk::trace::Recordable>> &spans,
    proto::collector::trace::v1::ExportTraceServiceRequest *request) noexcept
{
  if (nullptr == request)
  {
    return;
  }

  auto resource_span       = request->add_resource_spans();
  auto instrumentation_lib = resource_span->add_instrumentation_library_spans();
  bool first_pass          = true;

  for (auto &recordable : spans)
  {
    auto rec = std::unique_ptr<OtlpRecordable>(static_cast<OtlpRecordable *>(recordable.release()));
    *instrumentation_lib->add_spans()                       = std::move(rec->span());
    *instrumentation_lib->mutable_instrumentation_library() = rec->GetProtoInstrumentationLibrary();

    if (first_pass)
    {
      instrumentation_lib->set_schema_url(rec->GetInstrumentationLibrarySchemaURL());

      *resource_span->mutable_resource() = rec->ProtoResource();
      resource_span->set_schema_url(rec->GetResourceSchemaURL());

      first_pass = false;
    }
  }
}

#ifdef ENABLE_LOGS_PREVIEW
void OtlpRecordableUtils::PopulateRequest(
    const nostd::span<std::unique_ptr<opentelemetry::sdk::logs::Recordable>> &logs,
    proto::collector::logs::v1::ExportLogsServiceRequest *request) noexcept
{
  if (nullptr == request)
  {
    return;
  }

  for (auto &recordable : logs)
  {
    auto resource_logs       = request->add_resource_logs();
    auto instrumentation_lib = resource_logs->add_instrumentation_library_logs();

    auto rec =
        std::unique_ptr<OtlpLogRecordable>(static_cast<OtlpLogRecordable *>(recordable.release()));

    // TODO schema url
    *resource_logs->mutable_resource() = rec->ProtoResource();

    // TODO schema url
    // resource_logs->set_schema_url(rec->GetResourceSchemaURL());

    *instrumentation_lib->add_logs() = std::move(rec->log_record());
    // TODO instrumentation_library
    // *instrumentation_lib->mutable_instrumentation_library() =
    // rec->GetProtoInstrumentationLibrary();
    // TODO schema data
    // instrumentation_lib->set_schema_url(rec->GetInstrumentationLibrarySchemaURL());
  }
}
#endif

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
