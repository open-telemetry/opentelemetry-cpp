#pragma once

#include "opentelemetry/trace/tracer.h"

namespace opentelemetry
{
namespace sdk
{
namespace trace
{
class Tracer : public opentelemetry::trace::Tracer
{
public:
  explicit Tracer(nostd::string_view name, nostd::string_view version);
  nostd::unique_ptr<opentelemetry::trace::Span> StartSpan(nostd::string_view name,
                                            const opentelemetry::trace::StartSpanOptions &options = {}) noexcept override;
private:
  const nostd::string_view &name_;
  const nostd::string_view &version_;
};
}  // namespace trace
}  // namespace sdk
}  // namespace opentelemetry
