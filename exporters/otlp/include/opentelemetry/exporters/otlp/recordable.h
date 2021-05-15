#pragma once

#include "opentelemetry/exporters/otlp/protobuf_include_prefix.h"
#include "opentelemetry/proto/resource/v1/resource.pb.h"
#include "opentelemetry/proto/trace/v1/trace.pb.h"

#include "opentelemetry/exporters/otlp/protobuf_include_suffix.h"

#include "opentelemetry/sdk/trace/recordable.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{
class Recordable final : public sdk::trace::Recordable
{
public:
  const proto::trace::v1::Span &span() const noexcept { return span_; }

  /** Dynamically converts the resource of this span into a proto. */
  void PopulateProtoResource(proto::resource::v1::Resource *resource) const noexcept;

  void SetIdentity(const opentelemetry::trace::SpanContext &span_context,
                   opentelemetry::trace::SpanId parent_span_id) noexcept override;

  void SetAttribute(nostd::string_view key,
                    const opentelemetry::common::AttributeValue &value) noexcept override;

  void AddEvent(nostd::string_view name,
                common::SystemTimestamp timestamp,
                const common::KeyValueIterable &attributes) noexcept override;

  void AddLink(const opentelemetry::trace::SpanContext &span_context,
               const common::KeyValueIterable &attributes) noexcept override;

  void SetStatus(trace::StatusCode code, nostd::string_view description) noexcept override;

  void SetName(nostd::string_view name) noexcept override;

  void SetSpanKind(opentelemetry::trace::SpanKind span_kind) noexcept override;

  void SetResource(const opentelemetry::sdk::resource::Resource &resource) noexcept override;

  void SetStartTime(opentelemetry::common::SystemTimestamp start_time) noexcept override;

  void SetDuration(std::chrono::nanoseconds duration) noexcept override;

  void SetInstrumentationLibrary(
      const opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary
          &instrumentation_library) noexcept override;

private:
  proto::trace::v1::Span span_;
  const opentelemetry::sdk::resource::Resource *resource_ = nullptr;
};
}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
