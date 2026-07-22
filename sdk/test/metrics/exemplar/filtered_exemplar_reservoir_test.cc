// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW

#  include <gtest/gtest.h>
#  include <stdint.h>
#  include <chrono>
#  include <string>
#  include <vector>

#  include "opentelemetry/common/timestamp.h"
#  include "opentelemetry/context/context.h"
#  include "opentelemetry/nostd/shared_ptr.h"
#  include "opentelemetry/nostd/span.h"
#  include "opentelemetry/sdk/metrics/data/exemplar_data.h"
#  include "opentelemetry/sdk/metrics/exemplar/filter_type.h"
#  include "opentelemetry/sdk/metrics/exemplar/reservoir.h"
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

class CountingReservoir : public metrics_sdk::ExemplarReservoir
{
public:
  void OfferMeasurement(int64_t,
                        const metrics_sdk::MetricAttributes &,
                        const context_api::Context &,
                        const opentelemetry::common::SystemTimestamp &) noexcept override
  {
    ++offered;
  }

  void OfferMeasurement(double,
                        const metrics_sdk::MetricAttributes &,
                        const context_api::Context &,
                        const opentelemetry::common::SystemTimestamp &) noexcept override
  {
    ++offered;
  }

  std::vector<std::shared_ptr<metrics_sdk::ExemplarData>> CollectAndReset(
      const metrics_sdk::MetricAttributes &) noexcept override
  {
    return {};
  }

  int offered = 0;
};

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

int ForwardedCount(metrics_sdk::ExemplarFilterType filter_type, const context_api::Context &context)
{
  auto *spy     = new CountingReservoir();
  auto filtered = metrics_sdk::ExemplarReservoir::GetSimpleFilteredExemplarReservoir(
      filter_type, nostd::shared_ptr<metrics_sdk::ExemplarReservoir>(spy));
  filtered->OfferMeasurement(1.0, metrics_sdk::MetricAttributes{}, context,
                             std::chrono::system_clock::now());
  return spy->offered;
}
}  // namespace

TEST(FilteredExemplarReservoir, AlwaysOnOffersMeasurement)
{
  EXPECT_EQ(ForwardedCount(metrics_sdk::ExemplarFilterType::kAlwaysOn, context_api::Context{}), 1);
}

TEST(FilteredExemplarReservoir, AlwaysOnOffersLongMeasurement)
{
  auto *spy     = new CountingReservoir();
  auto filtered = metrics_sdk::ExemplarReservoir::GetSimpleFilteredExemplarReservoir(
      metrics_sdk::ExemplarFilterType::kAlwaysOn,
      nostd::shared_ptr<metrics_sdk::ExemplarReservoir>(spy));
  filtered->OfferMeasurement(static_cast<int64_t>(1), metrics_sdk::MetricAttributes{},
                             context_api::Context{}, std::chrono::system_clock::now());
  EXPECT_EQ(spy->offered, 1);
}

TEST(FilteredExemplarReservoir, AlwaysOffDropsMeasurement)
{
  EXPECT_EQ(ForwardedCount(metrics_sdk::ExemplarFilterType::kAlwaysOff, context_api::Context{}), 0);
}

TEST(FilteredExemplarReservoir, TraceBasedDropsWithoutSpan)
{
  EXPECT_EQ(ForwardedCount(metrics_sdk::ExemplarFilterType::kTraceBased, context_api::Context{}),
            0);
}

TEST(FilteredExemplarReservoir, TraceBasedOffersWithSampledSpan)
{
  EXPECT_EQ(ForwardedCount(metrics_sdk::ExemplarFilterType::kTraceBased, ContextWithSpan(true)), 1);
}

TEST(FilteredExemplarReservoir, TraceBasedDropsUnsampledSpan)
{
  EXPECT_EQ(ForwardedCount(metrics_sdk::ExemplarFilterType::kTraceBased, ContextWithSpan(false)),
            0);
}

#endif  // ENABLE_METRICS_EXEMPLAR_PREVIEW
