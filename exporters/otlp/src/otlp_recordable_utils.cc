// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/otlp/otlp_recordable_utils.h"

#include "opentelemetry/exporters/otlp/protobuf_include_prefix.h"

#include "opentelemetry/proto/logs/v1/logs.pb.h"
#include "opentelemetry/proto/trace/v1/trace.pb.h"

#include "opentelemetry/exporters/otlp/protobuf_include_suffix.h"

#include "opentelemetry/exporters/otlp/otlp_log_recordable.h"
#include "opentelemetry/exporters/otlp/otlp_recordable.h"

#include <list>
#include <unordered_map>

namespace nostd = opentelemetry::nostd;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

namespace
{
struct InstrumentationLibraryPointerHasher
{
  std::size_t operator()(const opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary
                             *instrumentation) const noexcept
  {
    return instrumentation->HashCode();
  }
};

struct InstrumentationLibraryPointerEqual
{
  std::size_t operator()(
      const opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary *left,
      const opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary *right)
      const noexcept
  {
    return *left == *right;
  }
};
}  // namespace

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

  for (auto &recordable : spans)
  {
    auto rec = std::unique_ptr<OtlpRecordable>(static_cast<OtlpRecordable *>(recordable.release()));
    auto resource_span       = request->add_resource_spans();
    auto instrumentation_lib = resource_span->add_instrumentation_library_spans();

    *instrumentation_lib->add_spans()                       = std::move(rec->span());
    *instrumentation_lib->mutable_instrumentation_library() = rec->GetProtoInstrumentationLibrary();

    instrumentation_lib->set_schema_url(rec->GetInstrumentationLibrarySchemaURL());

    *resource_span->mutable_resource() = rec->ProtoResource();
    resource_span->set_schema_url(rec->GetResourceSchemaURL());
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

  using logs_index_by_instrumentation_type =
      std::unordered_map<const opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary *,
                         std::list<std::unique_ptr<OtlpLogRecordable>>,
                         InstrumentationLibraryPointerHasher, InstrumentationLibraryPointerEqual>;
  std::unordered_map<const opentelemetry::sdk::resource::Resource *,
                     logs_index_by_instrumentation_type>
      logs_index_by_resource;

  for (auto &recordable : logs)
  {
    auto rec =
        std::unique_ptr<OtlpLogRecordable>(static_cast<OtlpLogRecordable *>(recordable.release()));
    auto instrumentation = &rec->GetInstrumentationLibrary();
    auto resource        = &rec->GetResource();

    logs_index_by_resource[resource][instrumentation].emplace_back(std::move(rec));
  }

  for (auto &input_resource_log : logs_index_by_resource)
  {
    auto output_resource_log = request->add_resource_logs();
    for (auto &input_scope_log : input_resource_log.second)
    {
      auto output_scope_log = output_resource_log->add_scope_logs();
      for (auto &input_log_record : input_scope_log.second)
      {
        if (!output_resource_log->has_resource())
        {
          *output_resource_log->mutable_resource() = input_log_record->ProtoResource();
          output_resource_log->set_schema_url(input_resource_log.first->GetSchemaURL());
        }

        if (!output_scope_log->has_scope())
        {
          output_scope_log->mutable_scope()->set_name(input_scope_log.first->GetName());
          output_scope_log->mutable_scope()->set_version(input_scope_log.first->GetVersion());
          output_scope_log->set_schema_url(input_scope_log.first->GetSchemaURL());
        }

        *output_scope_log->add_log_records() = std::move(input_log_record->log_record());
      }
    }
  }
}
#endif

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
