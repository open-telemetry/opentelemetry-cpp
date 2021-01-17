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
Tracer::Tracer(std::shared_ptr<SpanProcessor> processor,
               std::shared_ptr<Sampler> sampler,
               const opentelemetry::sdk::resource::Resource &resource) noexcept
    : processor_{processor}, sampler_{sampler}, resource_{resource}
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

trace_api::SpanContext GetCurrentSpanContext(const trace_api::SpanContext &explicit_parent)
{
  // Use the explicit parent, if it's valid.
  if (explicit_parent.IsValid())
  {
    return explicit_parent;
  }

  // Use the currently active span, if there's one.
  auto curr_span_context = context::RuntimeContext::GetValue(trace_api::kSpanKey);

  if (nostd::holds_alternative<nostd::shared_ptr<trace_api::Span>>(curr_span_context))
  {
    auto curr_span = nostd::get<nostd::shared_ptr<trace_api::Span>>(curr_span_context);
    return curr_span->GetContext();
  }

  // Otherwise return an invalid SpanContext.
  return trace_api::SpanContext::GetInvalid();
}

nostd::shared_ptr<trace_api::Span> Tracer::StartSpan(
    nostd::string_view name,
    const opentelemetry::common::KeyValueIterable &attributes,
    const trace_api::SpanContextKeyValueIterable &links,
    const trace_api::StartSpanOptions &options) noexcept
{
  trace_api::SpanContext parent = GetCurrentSpanContext(options.parent);

  auto sampling_result =
      sampler_->ShouldSample(parent, parent.trace_id(), name, options.kind, attributes, links);
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
                                links, options, parent, resource_}};

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
