#include "opentelemetry/sdk/trace/tracer.h"

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

nostd::shared_ptr<trace_api::Span> Tracer::StartSpan(
    nostd::string_view name,
    const trace_api::KeyValueIterable &attributes,
    const trace_api::StartSpanOptions &options) noexcept
{
  // TODO: replace nullptr with parent context in span context
  auto sampling_result =
      sampler_->ShouldSample(nullptr, trace_api::TraceId(), name, options.kind, attributes);
  if (sampling_result.decision == Decision::NOT_RECORD)
  {
    auto span = nostd::shared_ptr<trace_api::Span>{
        new (std::nothrow) trace_api::NoopSpan{this->shared_from_this()}};

    span->token_    = new context::Token();
    *(span->token_) = context::RuntimeContext::Attach(
        context::RuntimeContext::GetCurrent().SetValue("span_key", span));

    return span;
  }
  else
  {
    auto span = nostd::shared_ptr<trace_api::Span>{new (std::nothrow) Span{
        this->shared_from_this(), processor_.load(), name, attributes, options}};

    span->token_ = new context::Token();

    *(span->token_) = context::RuntimeContext::Attach(
        context::RuntimeContext::GetCurrent().SetValue("span_key", span));

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
