#pragma once

#include <map>
#include <memory>
#include <string>

#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/sdk/trace/processor.h"
#include "opentelemetry/sdk/trace/tracer.h"
#include "opentelemetry/trace/tracer.h"
#include "opentelemetry/trace/tracer_provider.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
class TracerProvider final : public opentelemetry::trace::TracerProvider
{
public:
  TracerProvider(std::shared_ptr<SpanProcessor> processor) noexcept;

  opentelemetry::nostd::shared_ptr<opentelemetry::trace::Tracer> GetTracer(
      nostd::string_view library_name,
      nostd::string_view library_version = "") noexcept override;

  /**
   * Set the span processor associated with this tracer provider.
   * @param processor The new span processor for this tracer provider.
   */
  void SetProcessor(std::shared_ptr<SpanProcessor> processor) noexcept;

  /**
   * Obtain the span processor associated with this tracer provider.
   * @return The span processor for this tracer provider.
   */
  SpanProcessor& GetProcessor() const noexcept;

private:
  opentelemetry::nostd::shared_ptr<opentelemetry::trace::Tracer> tracer_;
  std::shared_ptr<SpanProcessor> processor_;
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
