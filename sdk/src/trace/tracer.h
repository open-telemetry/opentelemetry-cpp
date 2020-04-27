#pragma once

#include "opentelemetry/trace/tracer.h"
#include "opentelemetry/version.h"
#include "opentelemetry/sdk/trace/processor.h"

#include <memory>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
class Tracer final : public trace_api::Tracer, public std::enable_shared_from_this<Tracer>
{
public:
  // Note: processor must be non-null
  explicit Tracer(std::unique_ptr<SpanProcessor> &&processor) noexcept : processor_{std::move(processor)} {}

  SpanProcessor &processor() const noexcept { return *processor_; }

  // trace_api::Tracer
  nostd::unique_ptr<trace_api::Span> StartSpan(
      nostd::string_view name,
      const trace_api::StartSpanOptions &options = {}) noexcept override;

  void ForceFlushWithMicroseconds(uint64_t timeout) noexcept override;

  void CloseWithMicroseconds(uint64_t timeout) noexcept override;

private:
  std::unique_ptr<SpanProcessor> processor_;
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
