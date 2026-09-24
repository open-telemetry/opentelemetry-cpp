// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <chrono>
#include <cstddef>  // For std::size_t and std::max_align_t
#include <cstdint>  // For std::uint32_t
#include <limits>   // For std::numeric_limits
#include <string>

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
      : arena_{arena_initial_block_, sizeof(arena_initial_block_)},
        span_{google::protobuf::Arena::Create<proto::trace::v1::Span>(&arena_)},
        span_limits_{max_attributes,
                     (std::numeric_limits<std::size_t>::max)(),
                     max_events,
                     max_links,
                     max_attributes_per_event,
                     max_attributes_per_link}
  {}

  // The Arena member owns the memory the span message points into, and an Arena is neither
  // copyable nor movable, so neither is the recordable. Recordables are created and handed
  // around by pointer, so nothing in the SDK or the exporters needs these.
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
  // Size of the block the Arena starts from. The block lives inside the recordable, so a span
  // whose recorded content fits in it never asks the heap for Arena memory at all. A span with
  // the identity, the name, the kind and the timestamps recorded takes 288 bytes of Arena
  // space, so 512 covers that plus a little, and a span that outgrows it keeps working, the
  // Arena simply grows onto the heap from there. 512, 768 and 1024 were all measured end to
  // end through a full BatchSpanProcessor queue. Larger blocks buy fewer allocations for spans
  // with many long attributes, and for that shape a 768 byte block is actually ahead on both
  // allocation count and record time, and on peak memory too, since which size wins there
  // depends on where a span's overflow lands in protobuf's block doubling. What 512 buys is the
  // other two columns: it is the smallest size that keeps a minimal span's identity fields off
  // the heap, and it keeps the recordable small enough to avoid the regression seen at 768 and
  // 1024 in the cost of starting a span while many are alive. That regression is a property of
  // the allocator: on glibc malloc the boundary between the exact fit smallbins and the best
  // fit largebins falls at a 1008 byte chunk, so it is worth re-measuring on a different one.
  static constexpr std::size_t kArenaInitialBlockSize = 512;

  // Declared before arena_ so the block is a live subobject before the Arena is pointed at it,
  // and is still there when the Arena is destroyed. Deliberately left uninitialized, the Arena
  // hands it out as it fills it. protobuf never frees a caller supplied initial block.
  alignas(std::max_align_t) char arena_initial_block_[kArenaInitialBlockSize];
  // Declared before span_ so the Arena is constructed first and destroyed last. The span message
  // and everything recorded into it live on this Arena, so recording a field is an Arena bump
  // instead of a heap allocation, and the whole span is released with the Arena.
  google::protobuf::Arena arena_;
  // Owned by arena_, never null, never deleted.
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
