#pragma once

#include <map>
#include <memory>
#include <string>

#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/trace/tracer.h"
#include "opentelemetry/trace/tracer_provider.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
class DefaultTracerProvider final : public opentelemetry::trace::TracerProvider
{
public:
  DefaultTracerProvider() noexcept;

  opentelemetry::nostd::shared_ptr<opentelemetry::trace::Tracer> GetTracer(
      nostd::string_view library_name,
      nostd::string_view library_version = "") noexcept override;

private:
  opentelemetry::nostd::shared_ptr<opentelemetry::trace::Tracer> tracer_;
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
