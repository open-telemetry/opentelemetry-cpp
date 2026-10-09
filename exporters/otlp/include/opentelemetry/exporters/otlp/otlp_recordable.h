// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <chrono>
#include <cstddef>  // For std::size_t
#include <cstdint>  // For std::uint32_t
#include <limits>   // For std::numeric_limits
#include <memory>
#include <string>
#include <utility>

// clang-format off
#include "opentelemetry/exporters/otlp/protobuf_include_prefix.h" // IWYU pragma: keep
#include "google/protobuf/arena.h"
#include "opentelemetry/proto/common/v1/common.pb.h"
#include "opentelemetry/proto/resource/v1/resource.pb.h"
#include "opentelemetry/proto/trace/v1/trace.pb.h"
#include "opentelemetry/exporters/otlp/protobuf_include_suffix.h" // IWYU pragma: keep
// clang-format on

#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/common/key_value_iterable.h"
#include "opentelemetry/common/timestamp.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/instrumentationscope/instrumentation_scope.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/sdk/trace/recordable.h"
#include "opentelemetry/sdk/trace/span_limits.h"
#include "opentelemetry/trace/span_context.h"
#include "opentelemetry/trace/span_id.h"
#include "opentelemetry/trace/span_metadata.h"
#include "opentelemetry/trace/trace_flags.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{
class OtlpRecordable final : public opentelemetry::sdk::trace::Recordable
{
public:
  /**
   * Construct with optional span limits parameters.
   *
   * @deprecated Configure span limits via TracerProviderFactory::Create(), SpanLimitsConfiguration,
   * or the YAML `tracer_provider.limits` node instead. These optional span limit params will be
   * removed in a future release.
   */
  explicit OtlpRecordable(
      std::uint32_t max_attributes           = (std::numeric_limits<std::uint32_t>::max)(),
      std::uint32_t max_events               = (std::numeric_limits<std::uint32_t>::max)(),
      std::uint32_t max_links                = (std::numeric_limits<std::uint32_t>::max)(),
      std::uint32_t max_attributes_per_event = (std::numeric_limits<std::uint32_t>::max)(),
      std::uint32_t max_attributes_per_link  = (std::numeric_limits<std::uint32_t>::max)())
      : OtlpRecordable(nullptr,
                       max_attributes,
                       max_events,
                       max_links,
                       max_attributes_per_event,
                       max_attributes_per_link)
  {}

  /**
   * Construct on the given Arena, with optional span limits parameters.
   *
   * The span message is created on arena, and the recordable keeps a reference to it, so the
   * Arena outlives the recordable. The OTLP exporters pass the Arena they share across every
   * recordable they create between two exports, so that the export request can be created on
   * the same Arena and take the span message without a copy. A null arena gives the recordable
   * an Arena of its own.
   *
   * @deprecated The span limit params, see the constructor above.
   */
  explicit OtlpRecordable(
      std::shared_ptr<google::protobuf::Arena> arena,
      std::uint32_t max_attributes           = (std::numeric_limits<std::uint32_t>::max)(),
      std::uint32_t max_events               = (std::numeric_limits<std::uint32_t>::max)(),
      std::uint32_t max_links                = (std::numeric_limits<std::uint32_t>::max)(),
      std::uint32_t max_attributes_per_event = (std::numeric_limits<std::uint32_t>::max)(),
      std::uint32_t max_attributes_per_link  = (std::numeric_limits<std::uint32_t>::max)())
      : arena_{arena ? std::move(arena) : std::make_shared<google::protobuf::Arena>()},
        span_{google::protobuf::Arena::Create<proto::trace::v1::Span>(arena_.get())},
        span_limits_{max_attributes,
                     (std::numeric_limits<std::size_t>::max)(),
                     max_events,
                     max_links,
                     max_attributes_per_event,
                     max_attributes_per_link}
  {}

  // The span message is owned by the Arena, not by the recordable, and an export request on the
  // same Arena may hold it, so the recordable is neither copyable nor movable. Recordables are
  // created and handed around by pointer, so nothing in the SDK or the exporters needs these.
  OtlpRecordable(const OtlpRecordable &)            = delete;
  OtlpRecordable &operator=(const OtlpRecordable &) = delete;
  OtlpRecordable(OtlpRecordable &&)                 = delete;
  OtlpRecordable &operator=(OtlpRecordable &&)      = delete;

  proto::trace::v1::Span &span() noexcept { return *span_; }
  const proto::trace::v1::Span &span() const noexcept { return *span_; }

  /** Dynamically converts the resource of this span into a proto. */
  proto::resource::v1::Resource ProtoResource() const noexcept;

  const opentelemetry::sdk::resource::Resource *GetResource() const noexcept;
  const std::string GetResourceSchemaURL() const noexcept;
  const opentelemetry::sdk::instrumentationscope::InstrumentationScope *GetInstrumentationScope()
      const noexcept;
  const std::string GetInstrumentationLibrarySchemaURL() const noexcept;

  proto::common::v1::InstrumentationScope GetProtoInstrumentationScope() const noexcept;

  void SetIdentity(const opentelemetry::trace::SpanContext &span_context,
                   opentelemetry::trace::SpanId parent_span_id) noexcept override;

  void SetAttribute(opentelemetry::nostd::string_view key,
                    const opentelemetry::common::AttributeValue &value) noexcept override;

  void AddEvent(opentelemetry::nostd::string_view name,
                opentelemetry::common::SystemTimestamp timestamp,
                const opentelemetry::common::KeyValueIterable &attributes) noexcept override;

  void AddLink(const opentelemetry::trace::SpanContext &span_context,
               const opentelemetry::common::KeyValueIterable &attributes) noexcept override;

  void SetStatus(opentelemetry::trace::StatusCode code,
                 nostd::string_view description) noexcept override;

  void SetName(nostd::string_view name) noexcept override;

  void SetTraceFlags(opentelemetry::trace::TraceFlags flags) noexcept override;

  void SetSpanKind(opentelemetry::trace::SpanKind span_kind) noexcept override;

  void SetResource(const opentelemetry::sdk::resource::Resource &resource) noexcept override;

  void SetStartTime(opentelemetry::common::SystemTimestamp start_time) noexcept override;

  void SetDuration(std::chrono::nanoseconds duration) noexcept override;

  /**
   * Set span limits.
   *
   * Until the deprecated OTLP exporter option values are removed, the effective limit for each
   * field is the minimum (most restrictive) of the deprecated otlp exporter option values (passed
   * to the constructor) and the values set by calling this method.
   *
   * @param limits The span limits to set.
   */
  void SetSpanLimits(const opentelemetry::sdk::trace::SpanLimits &limits) noexcept override;

  void SetInstrumentationScope(const opentelemetry::sdk::instrumentationscope::InstrumentationScope
                                   &instrumentation_scope) noexcept override;

private:
  // Declared before span_ so the Arena is set before the span message is created on it. The Arena
  // may be shared with other recordables and with export requests, and it is destroyed by the last
  // of them, so the span message stays valid for as long as this recordable or a request built
  // from it is alive.
  std::shared_ptr<google::protobuf::Arena> arena_;
  // Owned by the Arena, never null, never deleted.
  proto::trace::v1::Span *span_;
  const opentelemetry::sdk::resource::Resource *resource_ = nullptr;
  const opentelemetry::sdk::instrumentationscope::InstrumentationScope *instrumentation_scope_ =
      nullptr;
  opentelemetry::sdk::trace::SpanLimits span_limits_{
      opentelemetry::sdk::trace::SpanLimits::NoLimits()};
};
}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
