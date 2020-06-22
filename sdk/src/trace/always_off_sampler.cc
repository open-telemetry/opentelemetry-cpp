#include "opentelemetry/sdk/trace/always_off_sampler.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
namespace trace_api = opentelemetry::trace;

AlwaysOffSampler::AlwaysOffSampler() noexcept
  : sampling_result_(new AlwaysOffSamplingResultImpl()) {}

SamplingResult AlwaysOffSampler::ShouldSample(
    std::shared_ptr<SpanContext> parent_context,
    trace_api::TraceId trace_id,
    nostd::string_view name,
    trace_api::SpanKind span_kind,
    const nostd::span<AttributeKeyValue> &attributes) noexcept override
  {
    return sampling_result_;
  }

std::string GetDescription() const noexcept override
{
  return "AlwaysOffSampler";
}

class AlwaysOffSamplingResultImpl : public SamplingResult
{
public:
  Decision GetDecision() const noexcept override
  {
    return decision_;
  }

  nostd::span<AttributeKeyValue> GetAttributes() const noexcept override
  {
    return attributes_;
  }

private:
  Decision decision_ = Decision::NOT_RECORD;
  nostd::span<AttributeKeyValue> attributes_ = nostd::span();
};