// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifdef ENABLE_LOGS_PREVIEW

// clang-format off
#  include "opentelemetry/exporters/otlp/protobuf_include_prefix.h"

#  include "opentelemetry/proto/logs/v1/logs.pb.h"
#  include "opentelemetry/proto/resource/v1/resource.pb.h"
#  include "opentelemetry/sdk/instrumentationlibrary/instrumentation_library.h"

#  include "opentelemetry/exporters/otlp/protobuf_include_suffix.h"
// clang-format on

#  include "opentelemetry/sdk/common/attribute_utils.h"
#  include "opentelemetry/sdk/logs/recordable.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

/**
 * An OTLP Recordable implemenation for Logs.
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
  void SetTimestamp(opentelemetry::common::SystemTimestamp timestamp) noexcept override;

  /**
   * Set the severity for this log.
   * @param severity the severity of the event
   */
  void SetSeverity(opentelemetry::logs::Severity severity) noexcept override;

  /**
   * Set body field for this log.
   * @param message the body to set
   */
  void SetBody(nostd::string_view message) noexcept override;

  /**
   * Set Resource of this log
   * @param Resource the resource to set
   */
  void SetResource(const opentelemetry::sdk::resource::Resource &resource) noexcept override;

  /** Returns the associated resource */
  const opentelemetry::sdk::resource::Resource &GetResource() const noexcept;

  /**
   * Set an attribute of a log.
   * @param key the name of the attribute
   * @param value the attribute value
   */
  void SetAttribute(nostd::string_view key,
                    const opentelemetry::common::AttributeValue &value) noexcept override;

  /**
   * Set the trace id for this log.
   * @param trace_id the trace id to set
   */
  void SetTraceId(opentelemetry::trace::TraceId trace_id) noexcept override;

  /**
   * Set the span id for this log.
   * @param span_id the span id to set
   */
  void SetSpanId(opentelemetry::trace::SpanId span_id) noexcept override;

  /**
   * Inject trace_flags for this log.
   * @param trace_flags the trace flags to set
   */
  void SetTraceFlags(opentelemetry::trace::TraceFlags trace_flags) noexcept override;

  /**
   * Set instrumentation_library for this log.
   * @param instrumentation_library the instrumentation library to set
   */
  void SetInstrumentationLibrary(
      const opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary
          &instrumentation_library) noexcept override;

  /** Returns the associated instruementation library */
  const opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary &
  GetInstrumentationLibrary() const noexcept;

private:
  proto::logs::v1::LogRecord log_record_;
  const opentelemetry::sdk::resource::Resource *resource_ = nullptr;
  // TODO shared resource
  // const opentelemetry::sdk::resource::Resource *resource_ = nullptr;
  const opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary
      *instrumentation_library_ = nullptr;
};

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE

#endif
