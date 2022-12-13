// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/otlp/otlp_recordable_utils.h"

#include "opentelemetry/exporters/otlp/protobuf_include_prefix.h"

#include "opentelemetry/proto/logs/v1/logs.pb.h"
#include "opentelemetry/proto/trace/v1/trace.pb.h"

#include "opentelemetry/exporters/otlp/protobuf_include_suffix.h"

#include "opentelemetry/exporters/otlp/otlp_log_recordable.h"
#include "opentelemetry/exporters/otlp/otlp_populate_attribute_utils.h"
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
struct InstrumentationScopePointerHasher
{
  std::size_t operator()(const opentelemetry::sdk::instrumentationscope::InstrumentationScope
                             *instrumentation) const noexcept
  {
    return instrumentation->HashCode();
  }
};

struct InstrumentationScopePointerEqual
{
  std::size_t operator()(
      const opentelemetry::sdk::instrumentationscope::InstrumentationScope *left,
      const opentelemetry::sdk::instrumentationscope::InstrumentationScope *right) const noexcept
  {
    return *left == *right;
  }
};
}  // namespace

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
    auto resource_span = request->add_resource_spans();
    auto scope_spans   = resource_span->add_scope_spans();

    *scope_spans->add_spans()     = std::move(rec->span());
    *scope_spans->mutable_scope() = rec->GetProtoInstrumentationScope();

    scope_spans->set_schema_url(rec->GetInstrumentationLibrarySchemaURL());

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
      std::unordered_map<const opentelemetry::sdk::instrumentationscope::InstrumentationScope *,
                         std::list<std::unique_ptr<OtlpLogRecordable>>,
                         InstrumentationScopePointerHasher, InstrumentationScopePointerEqual>;
  std::unordered_map<const opentelemetry::sdk::resource::Resource *,
                     logs_index_by_instrumentation_type>
      logs_index_by_resource;

  for (auto &recordable : logs)
  {
    auto rec =
        std::unique_ptr<OtlpLogRecordable>(static_cast<OtlpLogRecordable *>(recordable.release()));
    auto instrumentation = &rec->GetInstrumentationScope();
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
          OtlpPopulateAttributeUtils::PopulateAttribute(output_resource_log->mutable_resource(),
                                                        *input_resource_log.first);
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
