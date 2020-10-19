#pragma once

#include "opentelemetry/proto/trace/v1/trace.pb.h"
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

  void SetIds(trace::TraceId trace_id,
              trace::SpanId span_id,
              trace::SpanId parent_span_id) noexcept override;

  void SetAttribute(nostd::string_view key,
                    const opentelemetry::common::AttributeValue &value) noexcept override;

  void AddEvent(nostd::string_view name,
                core::SystemTimestamp timestamp,
                const common::KeyValueIterable &attributes) noexcept override;

  void AddLink(opentelemetry::trace::SpanContext span_context,
               const common::KeyValueIterable &attributes) noexcept override;

  void SetStatus(trace::CanonicalCode code, nostd::string_view description) noexcept override;

  void SetName(nostd::string_view name) noexcept override;

  void SetStartTime(opentelemetry::core::SystemTimestamp start_time) noexcept override;

  void SetDuration(std::chrono::nanoseconds duration) noexcept override;

private:
  proto::trace::v1::Span span_;
};
}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
