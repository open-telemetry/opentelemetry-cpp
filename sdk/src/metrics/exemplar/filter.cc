// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/metrics/exemplar/filter.h"
#include "opentelemetry/sdk/metrics/exemplar/always_sample_filter.h"
#include "opentelemetry/sdk/metrics/exemplar/never_sample_filter.h"
#include "opentelemetry/sdk/metrics/exemplar/with_trace_sample_filter.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

std::shared_ptr<ExemplarFilter> ExemplarFilter::GetNeverSampleFilter() noexcept
{
  static std::shared_ptr<ExemplarFilter> neverSampleFilter{new NeverSampleFilter{}};
  return neverSampleFilter;
}

std::shared_ptr<ExemplarFilter> ExemplarFilter::GetAlwaysSampleFilter() noexcept
{
  static std::shared_ptr<ExemplarFilter> alwaysSampleFilter{new AlwaysSampleFilter{}};
  return alwaysSampleFilter;
}

std::shared_ptr<ExemplarFilter> ExemplarFilter::GetWithTraceSampleFilter() noexcept
{
  std::shared_ptr<ExemplarFilter> withTraceSampleFilter{new WithTraceSampleFilter{}};
  return withTraceSampleFilter;
}

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
