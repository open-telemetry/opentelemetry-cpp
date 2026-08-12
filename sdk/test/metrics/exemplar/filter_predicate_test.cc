// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW

#  include <gtest/gtest.h>
#  include <stdint.h>
#  include <string>

#  include "opentelemetry/context/context.h"
#  include "opentelemetry/nostd/shared_ptr.h"
#  include "opentelemetry/nostd/span.h"
#  include "opentelemetry/sdk/metrics/exemplar/filter_predicate.h"
#  include "opentelemetry/sdk/metrics/exemplar/filter_type.h"
#  include "opentelemetry/trace/context.h"
#  include "opentelemetry/trace/default_span.h"
#  include "opentelemetry/trace/span.h"
#  include "opentelemetry/trace/span_context.h"
#  include "opentelemetry/trace/span_id.h"
#  include "opentelemetry/trace/trace_flags.h"
#  include "opentelemetry/trace/trace_id.h"

namespace
{
namespace metrics_sdk = opentelemetry::sdk::metrics;
namespace trace_api   = opentelemetry::trace;
namespace context_api = opentelemetry::context;
namespace nostd       = opentelemetry::nostd;

context_api::Context ContextWithSpan(bool sampled)
{
  const uint8_t trace_id_bytes[trace_api::TraceId::kSize] = {1, 2,  3,  4,  5,  6,  7,  8,
                                                             9, 10, 11, 12, 13, 14, 15, 16};
  const uint8_t span_id_bytes[trace_api::SpanId::kSize]   = {1, 2, 3, 4, 5, 6, 7, 8};
  trace_api::SpanContext span_context(
      trace_api::TraceId(trace_id_bytes), trace_api::SpanId(span_id_bytes),
      sampled ? trace_api::TraceFlags(trace_api::TraceFlags::kIsSampled) : trace_api::TraceFlags(),
      false);
  nostd::shared_ptr<trace_api::Span> span(new trace_api::DefaultSpan(span_context));
  context_api::Context context;
  return trace_api::SetSpan(context, span);
}
}  // namespace

TEST(ExemplarFilterEnabled, AlwaysOnIsEligibleWithoutASpan)
{
  EXPECT_TRUE(metrics_sdk::ExemplarFilterEnabled(metrics_sdk::ExemplarFilterType::kAlwaysOn,
                                                 context_api::Context{}));
}

TEST(ExemplarFilterEnabled, AlwaysOffIsNeverEligible)
{
  EXPECT_FALSE(metrics_sdk::ExemplarFilterEnabled(metrics_sdk::ExemplarFilterType::kAlwaysOff,
                                                  ContextWithSpan(true)));
}

TEST(ExemplarFilterEnabled, TraceBasedRequiresSampledSpan)
{
  EXPECT_TRUE(metrics_sdk::ExemplarFilterEnabled(metrics_sdk::ExemplarFilterType::kTraceBased,
                                                 ContextWithSpan(true)));
  EXPECT_FALSE(metrics_sdk::ExemplarFilterEnabled(metrics_sdk::ExemplarFilterType::kTraceBased,
                                                  ContextWithSpan(false)));
  EXPECT_FALSE(metrics_sdk::ExemplarFilterEnabled(metrics_sdk::ExemplarFilterType::kTraceBased,
                                                  context_api::Context{}));
}

#endif  // ENABLE_METRICS_EXEMPLAR_PREVIEW
