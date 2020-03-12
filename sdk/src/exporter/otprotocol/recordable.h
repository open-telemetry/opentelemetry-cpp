#pragma once

#include "src/trace/recordable.h"

#include "opentelemetry/proto/trace/v1/trace.pb.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace exporter
{
namespace otprotocol
{
class Recordable final : public sdk::trace::Recordable
{
public:
  const proto::trace::v1::Span &span() const noexcept { return span_; }

  // sdk::trace::Recordable
  void AddEvent(nostd::string_view name, core::SystemTimestamp timestamp) noexcept override;

  void SetStatus(trace_api::CanonicalCode code, nostd::string_view description) noexcept override;

  void SetName(nostd::string_view name) noexcept override;

private:
  proto::trace::v1::Span span_;
};
}  // namespace otprotocol
}  // namespace exporter
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
