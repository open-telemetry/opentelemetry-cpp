// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifdef ENABLE_LOGS_PREVIEW

#  include "opentelemetry/exporters/otlp/protobuf_include_prefix.h"

#  include "opentelemetry/proto/logs/v1/logs.pb.h"
#  include "opentelemetry/proto/resource/v1/resource.pb.h"

#  include "opentelemetry/exporters/otlp/protobuf_include_suffix.h"

#  include "opentelemetry/sdk/logs/recordable.h"
#  include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

/**
 * An OTLP Recordable implemenation
 */
class OtlpLogRecordable final : public opentelemetry::sdk::logs::Recordable
{
public:
  virtual ~OtlpLogRecordable() = default;

  proto::logs::v1::LogRecord &log_record() noexcept { return log_record_; }
  const proto::logs::v1::LogRecord &log_record() const noexcept { return log_record_; }

  /** Dynamically converts the resource of this log into a proto. */
  proto::resource::v1::Resource ProtoResource() const noexcept;

  /**
   * Set the timestamp for this log.
   * @param timestamp the timestamp to set
   */
  virtual void SetTimestamp(opentelemetry::common::SystemTimestamp timestamp) noexcept override;

  /**
   * Set the severity for this log.
   * @param severity the severity of the event
   */
  virtual void SetSeverity(opentelemetry::logs::Severity severity) noexcept override;

  /**
   * Set name for this log
   * @param name the name to set
   */
  virtual void SetName(nostd::string_view name) noexcept override;

  /**
   * Set body field for this log.
   * @param message the body to set
   */
  virtual void SetBody(nostd::string_view message) noexcept override;

  /**
   * Set a single resource of a log record.
   * @param key the name of the resource to set
   * @param value the resource value to set
   */
  virtual void SetResource(nostd::string_view key,
                           const opentelemetry::common::AttributeValue &value) noexcept override;

  /**
   * Set an attribute of a log.
   * @param key the name of the attribute
   * @param value the attribute value
   */
  virtual void SetAttribute(nostd::string_view key,
                            const opentelemetry::common::AttributeValue &value) noexcept override;

  /**
   * Set the trace id for this log.
   * @param trace_id the trace id to set
   */
  virtual void SetTraceId(opentelemetry::trace::TraceId trace_id) noexcept override;

  /**
   * Set the span id for this log.
   * @param span_id the span id to set
   */
  virtual void SetSpanId(opentelemetry::trace::SpanId span_id) noexcept override;

  /**
   * Inject trace_flags for this log.
   * @param trace_flags the trace flags to set
   */
  virtual void SetTraceFlags(opentelemetry::trace::TraceFlags trace_flags) noexcept override;

private:
  proto::logs::v1::LogRecord log_record_;
  proto::resource::v1::Resource private_resource_;
  // TODO shared resource
  // const opentelemetry::sdk::resource::Resource *resource_ = nullptr;
  // TODO InstrumentationLibrary
  // const opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary
  //     *instrumentation_library_ = nullptr;
};
}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
#endif
