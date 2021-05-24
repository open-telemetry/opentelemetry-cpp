// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/sdk/common/atomic_shared_ptr.h"
#include "opentelemetry/sdk/trace/sampler.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
namespace trace_api = opentelemetry::trace;

/**
 * The ParentBased sampler is a composite sampler. ParentBased(delegateSampler) either respects
 * the parent span's sampling decision or delegates to delegateSampler for root spans.
 */
class ParentBasedSampler : public Sampler
{
public:
  explicit ParentBasedSampler(std::shared_ptr<Sampler> delegate_sampler) noexcept;
  /** The decision either respects the parent span's sampling decision or delegates to
   * delegateSampler for root spans
   * @return Returns DROP always
   */
  SamplingResult ShouldSample(
      const trace_api::SpanContext &parent_context,
      trace_api::TraceId trace_id,
      nostd::string_view name,
      trace_api::SpanKind span_kind,
      const opentelemetry::common::KeyValueIterable &attributes,
      const trace_api::SpanContextKeyValueIterable &links) noexcept override;

  /**
   * @return Description MUST be ParentBased{delegate_sampler_.getDescription()}
   */
  nostd::string_view GetDescription() const noexcept override;

private:
  const std::shared_ptr<Sampler> delegate_sampler_;
  const std::string description_;
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
