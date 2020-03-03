#include "sdk/src/trace/span.h"

namespace opentelemetry
{
namespace sdk
{
namespace trace
{
Span::Span(std::shared_ptr<Tracer> &&tracer,
           nostd::string_view name,
           const trace_api::StartSpanOptions &options) noexcept
    : tracer_{std::move(tracer)}, recordable_{tracer_->recorder().MakeRecordable()}
{
  (void)name;
  (void)options;
}
}  // namespace trace
}  // namespace sdk
}  // namespace opentelemetry
