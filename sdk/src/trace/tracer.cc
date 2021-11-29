// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/trace/tracer.h"
#include "opentelemetry/context/runtime_context.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/sdk/common/atomic_shared_ptr.h"
#include "opentelemetry/trace/context.h"
#include "opentelemetry/version.h"
#include "src/trace/span.h"

#include <memory>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{

Tracer::Tracer(std::shared_ptr<sdk::trace::TracerContext> context,
               std::unique_ptr<InstrumentationLibrary> instrumentation_library) noexcept
    : instrumentation_library_{std::move(instrumentation_library)}, context_{context}
{}

nostd::shared_ptr<trace_api::Span> Tracer::StartSpan(
    nostd::string_view name,
    const opentelemetry::common::KeyValueIterable &attributes,
    const trace_api::SpanContextKeyValueIterable &links,
    const trace_api::StartSpanOptions &options) noexcept
{
  trace_api::SpanContext parent_context = GetCurrentSpan()->GetContext();
  if (nostd::holds_alternative<trace_api::SpanContext>(options.parent))
  {
    auto span_context = nostd::get<trace_api::SpanContext>(options.parent);
    if (span_context.IsValid())
    {
      parent_context = span_context;
    }
  }
  else if (nostd::holds_alternative<context::Context>(options.parent))
  {
    auto context = nostd::get<context::Context>(options.parent);
    // fetch span context from parent span stored in the context
    auto span_context = opentelemetry::trace::GetSpan(context)->GetContext();
    if (span_context.IsValid())
    {
      parent_context = span_context;
    }
  }

  trace_api::TraceId trace_id;
  trace_api::SpanId span_id = GetIdGenerator().GenerateSpanId();
  bool is_parent_span_valid = false;

  if (parent_context.IsValid())
  {
    trace_id             = parent_context.trace_id();
    is_parent_span_valid = true;
  }
  else
  {
    trace_id = GetIdGenerator().GenerateTraceId();
  }

  auto sampling_result = context_->GetSampler().ShouldSample(parent_context, trace_id, name,
                                                             options.kind, attributes, links);
  auto trace_flags     = sampling_result.decision == Decision::DROP
                         ? trace_api::TraceFlags{}
                         : trace_api::TraceFlags{trace_api::TraceFlags::kIsSampled};

  auto span_context = std::unique_ptr<trace_api::SpanContext>(new trace_api::SpanContext(
      trace_id, span_id, trace_flags, false,
      sampling_result.trace_state ? sampling_result.trace_state
                                  : is_parent_span_valid ? parent_context.trace_state()
                                                         : trace_api::TraceState::GetDefault()));

  if (sampling_result.decision == Decision::DROP)
  {
    // create no-op span with valid span-context.

    auto noop_span = nostd::shared_ptr<trace_api::Span>{
        new (std::nothrow) trace_api::NoopSpan(this->shared_from_this(), std::move(span_context))};
    return noop_span;
  }
  else
  {

    auto span = nostd::shared_ptr<trace_api::Span>{
        new (std::nothrow) Span{this->shared_from_this(), name, attributes, links, options,
                                parent_context, std::move(span_context)}};

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
