#pragma once

#include "opentelemetry/sdk/common/atomic_shared_ptr.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/sdk/trace/processor.h"
#include "opentelemetry/sdk/trace/samplers/always_on.h"
#include "opentelemetry/trace/noop.h"
#include "opentelemetry/trace/tracer.h"
#include "opentelemetry/version.h"

#include <memory>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{

// Forward Declaration due to bidirectional dependency
class TracerProvider;

class Tracer final : public trace_api::Tracer, public std::enable_shared_from_this<Tracer>
{
public:
  /**
   * Initialize a new tracer.
   * @param parent The `TraceProvider` which owns resource + pipeline for traces.
   */
  explicit Tracer(TracerProvider *provider) noexcept;

  nostd::shared_ptr<trace_api::Span> StartSpan(
      nostd::string_view name,
      const opentelemetry::common::KeyValueIterable &attributes,
      const trace_api::SpanContextKeyValueIterable &links,
      const trace_api::StartSpanOptions &options = {}) noexcept override;

  void ForceFlushWithMicroseconds(uint64_t timeout) noexcept override;

  void CloseWithMicroseconds(uint64_t timeout) noexcept override;

  /** Returns the provider that created this Tracer. */
  const TracerProvider &GetProvider() const { return *provider_; }

private:
  TracerProvider *provider_;
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
