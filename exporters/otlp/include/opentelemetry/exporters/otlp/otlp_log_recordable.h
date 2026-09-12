// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>  // For std::size_t and std::max_align_t
#include <cstdint>

#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/sdk/instrumentationscope/instrumentation_scope.h"
#include "opentelemetry/sdk/logs/log_record_limits.h"
#include "opentelemetry/sdk/logs/recordable.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/version.h"

// clang-format off
#include "opentelemetry/exporters/otlp/protobuf_include_prefix.h" // IWYU pragma: keep
#include "google/protobuf/arena.h"
#include "opentelemetry/proto/logs/v1/logs.pb.h"
#include "opentelemetry/exporters/otlp/protobuf_include_suffix.h" // IWYU pragma: keep
// clang-format on

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
  OtlpLogRecordable()
      : arena_{arena_initial_block_, sizeof(arena_initial_block_)},
        proto_record_{google::protobuf::Arena::Create<proto::logs::v1::LogRecord>(&arena_)}
  {}

  // The Arena member owns the memory the log record message points into, and an Arena is neither
  // copyable nor movable, so neither is the recordable. Recordables are created and handed
  // around by pointer, so nothing in the SDK or the exporters needs these.
  OtlpLogRecordable(const OtlpLogRecordable &)            = delete;
  OtlpLogRecordable &operator=(const OtlpLogRecordable &) = delete;
  OtlpLogRecordable(OtlpLogRecordable &&)                 = delete;
  OtlpLogRecordable &operator=(OtlpLogRecordable &&)      = delete;

  proto::logs::v1::LogRecord &log_record() noexcept { return *proto_record_; }
  const proto::logs::v1::LogRecord &log_record() const noexcept { return *proto_record_; }

  /** Returns the associated resource */
  const opentelemetry::sdk::resource::Resource &GetResource() const noexcept;

  /** Returns the associated instruementation scope */
  const opentelemetry::sdk::instrumentationscope::InstrumentationScope &GetInstrumentationScope()
      const noexcept;

  /**
   * Set the timestamp for this log.
   * @param timestamp the timestamp to set
   */
  void SetTimestamp(opentelemetry::common::SystemTimestamp timestamp) noexcept override;

  /**
   * Set the observed timestamp for this log.
   * @param timestamp the timestamp to set
   */
  void SetObservedTimestamp(opentelemetry::common::SystemTimestamp timestamp) noexcept override;

  /**
   * Set the severity for this log.
   * @param severity the severity of the event
   */
  void SetSeverity(opentelemetry::logs::Severity severity) noexcept override;

  /**
   * Set body field for this log.
   * @param message the body to set
   */
  void SetBody(const opentelemetry::common::AttributeValue &message) noexcept override;

  /**
   * @brief Set the Event Id for this log.
   * @param id the event Id to set
   * @param name  the event name to set
   */
  void SetEventId(int64_t /* id */, nostd::string_view event_name) noexcept override;

  /**
   * Set the trace id for this log.
   * @param trace_id the trace id to set
   */
  void SetTraceId(const opentelemetry::trace::TraceId &trace_id) noexcept override;

  /**
   * Set the span id for this log.
   * @param span_id the span id to set
   */
  void SetSpanId(const opentelemetry::trace::SpanId &span_id) noexcept override;

  /**
   * Inject trace_flags for this log.
   * @param trace_flags the trace flags to set
   */
  void SetTraceFlags(const opentelemetry::trace::TraceFlags &trace_flags) noexcept override;

  /**
   * Set an attribute of a log.
   * @param key the name of the attribute
   * @param value the attribute value
   */
  void SetAttribute(nostd::string_view key,
                    const opentelemetry::common::AttributeValue &value) noexcept override;

  /**
   * Apply attribute count and value length limits. Must be called before any
   * SetAttribute call to take effect. The limits are copied into this
   * recordable.
   */
  void SetLogRecordLimits(
      const opentelemetry::sdk::logs::LogRecordLimits &limits) noexcept override;

  /**
   * Set Resource of this log
   * @param Resource the resource to set
   */
  void SetResource(const opentelemetry::sdk::resource::Resource &resource) noexcept override;

  /**
   * Set instrumentation_scope for this log.
   * @param instrumentation_scope the instrumentation scope to set
   */
  void SetInstrumentationScope(const opentelemetry::sdk::instrumentationscope::InstrumentationScope
                                   &instrumentation_scope) noexcept override;

private:
  // Size of the block the Arena starts from. The block lives inside the recordable, so a log
  // record whose recorded content fits in it never asks the heap for Arena memory at all. See
  // the same constant in otlp_recordable.h, which explains how the size was chosen. 256, 512
  // and 768 were measured here, and 512 was the best of the three: it is the smallest that
  // keeps a minimal record's Arena entirely inside the recordable.
  static constexpr std::size_t kArenaInitialBlockSize = 512;

  // Declared before arena_ so the block is a live subobject before the Arena is pointed at it,
  // and is still there when the Arena is destroyed. Deliberately left uninitialized, the Arena
  // hands it out as it fills it. protobuf never frees a caller supplied initial block.
  alignas(std::max_align_t) char arena_initial_block_[kArenaInitialBlockSize];
  // Declared before proto_record_ so the Arena is constructed first and destroyed last. The log
  // record message and everything recorded into it live on this Arena, so recording a field is an
  // Arena bump instead of a heap allocation, and the whole record is released with the Arena.
  google::protobuf::Arena arena_;
  // Owned by arena_, never null, never deleted.
  proto::logs::v1::LogRecord *proto_record_;
  const opentelemetry::sdk::resource::Resource *resource_ = nullptr;
  const opentelemetry::sdk::instrumentationscope::InstrumentationScope *instrumentation_scope_ =
      nullptr;
  // Stored by value so the recordable does not depend on the limits object
  // outliving the LoggerContext that supplied it. Defaults to no limits; the
  // LoggerProvider wiring injects the configured limits via SetLogRecordLimits,
  // so a recordable used outside a provider does not cap attributes on its own.
  opentelemetry::sdk::logs::LogRecordLimits limits_ =
      opentelemetry::sdk::logs::LogRecordLimits::NoLimits();
};

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
