#include "opentelemetry/sdk/trace/tracer.h"

#include "opentelemetry/context/runtime_context.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/sdk/common/atomic_shared_ptr.h"
#include "opentelemetry/version.h"
#include "src/trace/span.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
Tracer::Tracer(std::shared_ptr<SpanProcessor> processor, std::shared_ptr<Sampler> sampler) noexcept
    : processor_{processor}, sampler_{sampler}
{}

void Tracer::SetProcessor(std::shared_ptr<SpanProcessor> processor) noexcept
{
  processor_.store(processor);
}

std::shared_ptr<SpanProcessor> Tracer::GetProcessor() const noexcept
{
  return processor_.load();
}

std::shared_ptr<Sampler> Tracer::GetSampler() const noexcept
{
  return sampler_;
}

// Helper function to extract the current span context from the runtime context.
// Returns an invalid span context if the runtime context doesn't contain a span.
trace_api::SpanContext GetCurrentSpanContext()
{
  context::ContextValue curr_span_context = context::RuntimeContext::GetValue(SpanKey);

  if (nostd::holds_alternative<nostd::shared_ptr<trace_api::Span>>(curr_span_context))
  {
    auto curr_span = nostd::get<nostd::shared_ptr<trace_api::Span>>(curr_span_context);
    return curr_span->GetContext();
  }
  return trace_api::SpanContext();
}

nostd::shared_ptr<trace_api::Span> Tracer::StartSpan(
    nostd::string_view name,
    const trace_api::KeyValueIterable &attributes,
    const trace_api::StartSpanOptions &options,
    const nostd::span<trace_api::Link> &links) noexcept
{
  // TODO: replace nullptr with parent context in span context
  auto sampling_result =
      sampler_->ShouldSample(nullptr, trace_api::TraceId(), name, options.kind, attributes);
  if (sampling_result.decision == Decision::DROP)
  {
    // Don't allocate a no-op span for every DROP decision, but use a static
    // singleton for this case.
    static nostd::shared_ptr<trace_api::Span> noop_span(
        new trace_api::NoopSpan{this->shared_from_this()});

    return noop_span;
  }
  else
  {
    auto span = nostd::shared_ptr<trace_api::Span>{
        new (std::nothrow) Span{this->shared_from_this(), processor_.load(), name, attributes,
                                options, links, GetCurrentSpanContext()}};

    // if the attributes is not nullptr, add attributes to the span.
    if (sampling_result.attributes)
    {
      for (auto &kv : *sampling_result.attributes)
      {
        span->SetAttribute(kv.first, kv.second);
      }
    }

    return span;
  }
}

void Tracer::ForceFlushWithMicroseconds(uint64_t timeout) noexcept
{
  (void)timeout;
}

void Tracer::CloseWithMicroseconds(uint64_t timeout) noexcept
{
  (void)timeout;
}
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
