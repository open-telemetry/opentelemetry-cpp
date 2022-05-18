// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_LOGS_PREVIEW

#  include "opentelemetry/common/macros.h"

#  include "opentelemetry/exporters/otlp/otlp_log_recordable.h"

#  include "opentelemetry/exporters/otlp/otlp_recordable_utils.h"

namespace nostd = opentelemetry::nostd;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

proto::resource::v1::Resource OtlpLogRecordable::ProtoResource() const noexcept
{
  proto::resource::v1::Resource proto;
  if (nullptr == resource_)
  {
    OtlpRecordableUtils::PopulateAttribute(&proto, sdk::resource::Resource::GetDefault());
  }
  else
  {
    OtlpRecordableUtils::PopulateAttribute(&proto, *resource_);
  }

  return proto;
}

void OtlpLogRecordable::SetTimestamp(opentelemetry::common::SystemTimestamp timestamp) noexcept
{
  log_record_.set_time_unix_nano(timestamp.time_since_epoch().count());
}

void OtlpLogRecordable::SetSeverity(opentelemetry::logs::Severity severity) noexcept
{
  switch (severity)
  {
    case opentelemetry::logs::Severity::kTrace: {
      log_record_.set_severity_text("TRACE");
      log_record_.set_severity_number(proto::logs::v1::SEVERITY_NUMBER_TRACE);
      break;
    }
    case opentelemetry::logs::Severity::kTrace2: {
      log_record_.set_severity_text("TRACE2");
      log_record_.set_severity_number(proto::logs::v1::SEVERITY_NUMBER_TRACE2);
      break;
    }
    case opentelemetry::logs::Severity::kTrace3: {
      log_record_.set_severity_text("TRACE3");
      log_record_.set_severity_number(proto::logs::v1::SEVERITY_NUMBER_TRACE3);
      break;
    }
    case opentelemetry::logs::Severity::kTrace4: {
      log_record_.set_severity_text("TRACE4");
      log_record_.set_severity_number(proto::logs::v1::SEVERITY_NUMBER_TRACE4);
      break;
    }
    case opentelemetry::logs::Severity::kDebug: {
      log_record_.set_severity_text("DEBUG");
      log_record_.set_severity_number(proto::logs::v1::SEVERITY_NUMBER_DEBUG);
      break;
    }
    case opentelemetry::logs::Severity::kDebug2: {
      log_record_.set_severity_text("DEBUG2");
      log_record_.set_severity_number(proto::logs::v1::SEVERITY_NUMBER_DEBUG2);
      break;
    }
    case opentelemetry::logs::Severity::kDebug3: {
      log_record_.set_severity_text("DEBUG3");
      log_record_.set_severity_number(proto::logs::v1::SEVERITY_NUMBER_DEBUG3);
      break;
    }
    case opentelemetry::logs::Severity::kDebug4: {
      log_record_.set_severity_text("DEBUG4");
      log_record_.set_severity_number(proto::logs::v1::SEVERITY_NUMBER_DEBUG4);
      break;
    }
    case opentelemetry::logs::Severity::kInfo: {
      log_record_.set_severity_text("INFO");
      log_record_.set_severity_number(proto::logs::v1::SEVERITY_NUMBER_INFO);
      break;
    }
    case opentelemetry::logs::Severity::kInfo2: {
      log_record_.set_severity_text("INFO2");
      log_record_.set_severity_number(proto::logs::v1::SEVERITY_NUMBER_INFO2);
      break;
    }
    case opentelemetry::logs::Severity::kInfo3: {
      log_record_.set_severity_text("INFO3");
      log_record_.set_severity_number(proto::logs::v1::SEVERITY_NUMBER_INFO3);
      break;
    }
    case opentelemetry::logs::Severity::kInfo4: {
      log_record_.set_severity_text("INFO4");
      log_record_.set_severity_number(proto::logs::v1::SEVERITY_NUMBER_INFO4);
      break;
    }
    case opentelemetry::logs::Severity::kWarn: {
      log_record_.set_severity_text("WARN");
      log_record_.set_severity_number(proto::logs::v1::SEVERITY_NUMBER_WARN);
      break;
    }
    case opentelemetry::logs::Severity::kWarn2: {
      log_record_.set_severity_text("WARN2");
      log_record_.set_severity_number(proto::logs::v1::SEVERITY_NUMBER_WARN2);
      break;
    }
    case opentelemetry::logs::Severity::kWarn3: {
      log_record_.set_severity_text("WARN3");
      log_record_.set_severity_number(proto::logs::v1::SEVERITY_NUMBER_WARN3);
      break;
    }
    case opentelemetry::logs::Severity::kWarn4: {
      log_record_.set_severity_text("WARN4");
      log_record_.set_severity_number(proto::logs::v1::SEVERITY_NUMBER_WARN4);
      break;
    }
    case opentelemetry::logs::Severity::kError: {
      log_record_.set_severity_text("ERROR");
      log_record_.set_severity_number(proto::logs::v1::SEVERITY_NUMBER_ERROR);
      break;
    }
    case opentelemetry::logs::Severity::kError2: {
      log_record_.set_severity_text("ERROR2");
      log_record_.set_severity_number(proto::logs::v1::SEVERITY_NUMBER_ERROR2);
      break;
    }
    case opentelemetry::logs::Severity::kError3: {
      log_record_.set_severity_text("ERROR3");
      log_record_.set_severity_number(proto::logs::v1::SEVERITY_NUMBER_ERROR3);
      break;
    }
    case opentelemetry::logs::Severity::kError4: {
      log_record_.set_severity_text("ERROR4");
      log_record_.set_severity_number(proto::logs::v1::SEVERITY_NUMBER_ERROR4);
      break;
    }
    case opentelemetry::logs::Severity::kFatal: {
      log_record_.set_severity_text("FATAL");
      log_record_.set_severity_number(proto::logs::v1::SEVERITY_NUMBER_FATAL);
      break;
    }
    case opentelemetry::logs::Severity::kFatal2: {
      log_record_.set_severity_text("FATAL2");
      log_record_.set_severity_number(proto::logs::v1::SEVERITY_NUMBER_FATAL2);
      break;
    }
    case opentelemetry::logs::Severity::kFatal3: {
      log_record_.set_severity_text("FATAL3");
      log_record_.set_severity_number(proto::logs::v1::SEVERITY_NUMBER_FATAL3);
      break;
    }
    case opentelemetry::logs::Severity::kFatal4: {
      log_record_.set_severity_text("FATAL4");
      log_record_.set_severity_number(proto::logs::v1::SEVERITY_NUMBER_FATAL4);
      break;
    }
    default: {
      log_record_.set_severity_text("INVALID");
      log_record_.set_severity_number(proto::logs::v1::SEVERITY_NUMBER_UNSPECIFIED);
      break;
    }
  }
}

void OtlpLogRecordable::SetBody(nostd::string_view message) noexcept
{
  log_record_.mutable_body()->set_string_value(message.data(), message.size());
}

void OtlpLogRecordable::SetResource(const opentelemetry::sdk::resource::Resource &resource) noexcept
{
  resource_ = &resource;
}

const opentelemetry::sdk::resource::Resource &OtlpLogRecordable::GetResource() const noexcept
{
  OPENTELEMETRY_LIKELY_IF(nullptr != resource_) { return *resource_; }

  return opentelemetry::sdk::resource::Resource::GetDefault();
}

void OtlpLogRecordable::SetAttribute(nostd::string_view key,
                                     const opentelemetry::common::AttributeValue &value) noexcept
{
  OtlpRecordableUtils::PopulateAttribute(log_record_.add_attributes(), key, value);
}

void OtlpLogRecordable::SetTraceId(opentelemetry::trace::TraceId trace_id) noexcept
{
  log_record_.set_trace_id(reinterpret_cast<const char *>(trace_id.Id().data()),
                           trace::TraceId::kSize);
}

void OtlpLogRecordable::SetSpanId(opentelemetry::trace::SpanId span_id) noexcept
{
  log_record_.set_span_id(reinterpret_cast<const char *>(span_id.Id().data()),
                          trace::SpanId::kSize);
}

void OtlpLogRecordable::SetTraceFlags(opentelemetry::trace::TraceFlags trace_flags) noexcept
{
  log_record_.set_flags(trace_flags.flags());
}

void OtlpLogRecordable::SetInstrumentationLibrary(
    const opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary
        &instrumentation_library) noexcept
{
  instrumentation_library_ = &instrumentation_library;
}

const opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary &
OtlpLogRecordable::GetInstrumentationLibrary() const noexcept
{
  OPENTELEMETRY_LIKELY_IF(nullptr != instrumentation_library_) { return *instrumentation_library_; }

  static opentelemetry::nostd::unique_ptr<
      opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary>
      default_instrumentation =
          opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary::Create(
              "default", "1.0.0", "https://opentelemetry.io/schemas/1.11.0");
  return *default_instrumentation;
}
}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE

#endif
