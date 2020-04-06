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

  // sdk::trace::Recordable
  void AddEvent(nostd::string_view name, core::SystemTimestamp timestamp) noexcept override;

  void SetStatus(trace::CanonicalCode code, nostd::string_view description) noexcept override;

  void SetName(nostd::string_view name) noexcept override;

private:
  proto::trace::v1::Span span_;
};
}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
