// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <chrono>
#include <string>

// clang-format off
#include "opentelemetry/exporters/otlp/protobuf_include_prefix.h" // IWYU pragma: keep
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
  explicit OtlpRecordable(uint32_t max_attributes           = UINT32_MAX,
                          uint32_t max_events               = UINT32_MAX,
                          uint32_t max_links                = UINT32_MAX,
                          uint32_t max_attributes_per_event = UINT32_MAX,
                          uint32_t max_attributes_per_link  = UINT32_MAX)
      : max_attributes_(max_attributes),
        max_events_(max_events),
        max_links_(max_links),
        max_attributes_per_event_(max_attributes_per_event),
        max_attributes_per_link_(max_attributes_per_link)
  {}

  proto::trace::v1::Span &span() noexcept { return span_; }
  const proto::trace::v1::Span &span() const noexcept { return span_; }

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

  void SetInstrumentationScope(const opentelemetry::sdk::instrumentationscope::InstrumentationScope
                                   &instrumentation_scope) noexcept override;

private:
  proto::trace::v1::Span span_;
  const opentelemetry::sdk::resource::Resource *resource_ = nullptr;
  const opentelemetry::sdk::instrumentationscope::InstrumentationScope *instrumentation_scope_ =
      nullptr;
  uint32_t max_attributes_;
  uint32_t max_events_;
  uint32_t max_links_;
  uint32_t max_attributes_per_event_;
  uint32_t max_attributes_per_link_;
};
}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
