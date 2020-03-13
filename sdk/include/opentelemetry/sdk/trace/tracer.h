#pragma once

#include "opentelemetry/trace/tracer.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
class Tracer : public opentelemetry::trace::Tracer
{
public:
  nostd::unique_ptr<opentelemetry::trace::Span> StartSpan(
      nostd::string_view name,
      const opentelemetry::trace::StartSpanOptions &options = {}) noexcept override;
};

}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
