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
#  include "opentelemetry/sdk/metrics/exemplar/reservoir_cell.h"
#  include "opentelemetry/sdk/metrics/exemplar/simple_fixed_size_exemplar_reservoir.h"
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

nostd::shared_ptr<metrics_sdk::ExemplarReservoir> MakeFiltered(
    metrics_sdk::ExemplarFilterType filter_type,
    metrics_sdk::MapAndResetCellType map_and_reset = &metrics_sdk::ReservoirCell::GetAndResetDouble)
{
  auto inner = metrics_sdk::ExemplarReservoir::GetSimpleFixedSizeExemplarReservoir(
      1, metrics_sdk::SimpleFixedSizeExemplarReservoir::GetSimpleFixedSizeCellSelector(),
      map_and_reset);
  return metrics_sdk::ExemplarReservoir::GetSimpleFilteredExemplarReservoir(filter_type, inner);
}

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

bool OffersDouble(const nostd::shared_ptr<metrics_sdk::ExemplarReservoir> &reservoir,
                  const context_api::Context &context)
{
  reservoir->OfferMeasurement(1.0, metrics_sdk::MetricAttributes{}, context,
                              std::chrono::system_clock::now());
  return !reservoir->CollectAndReset(metrics_sdk::MetricAttributes{}).empty();
}
}  // namespace

TEST(FilteredExemplarReservoir, AlwaysOnOffersMeasurement)
{
  EXPECT_TRUE(OffersDouble(MakeFiltered(metrics_sdk::ExemplarFilterType::kAlwaysOn),
                           context_api::Context{}));
}

TEST(FilteredExemplarReservoir, AlwaysOnOffersLongMeasurement)
{
  auto reservoir = MakeFiltered(metrics_sdk::ExemplarFilterType::kAlwaysOn,
                                &metrics_sdk::ReservoirCell::GetAndResetLong);
  reservoir->OfferMeasurement(static_cast<int64_t>(1), metrics_sdk::MetricAttributes{},
                              context_api::Context{}, std::chrono::system_clock::now());
  EXPECT_FALSE(reservoir->CollectAndReset(metrics_sdk::MetricAttributes{}).empty());
}

TEST(FilteredExemplarReservoir, AlwaysOffDropsMeasurement)
{
  EXPECT_FALSE(OffersDouble(MakeFiltered(metrics_sdk::ExemplarFilterType::kAlwaysOff),
                            context_api::Context{}));
}

TEST(FilteredExemplarReservoir, TraceBasedDropsWithoutSpan)
{
  EXPECT_FALSE(OffersDouble(MakeFiltered(metrics_sdk::ExemplarFilterType::kTraceBased),
                            context_api::Context{}));
}

TEST(FilteredExemplarReservoir, TraceBasedOffersWithSampledSpan)
{
  EXPECT_TRUE(OffersDouble(MakeFiltered(metrics_sdk::ExemplarFilterType::kTraceBased),
                           ContextWithSpan(true)));
}

TEST(FilteredExemplarReservoir, TraceBasedDropsUnsampledSpan)
{
  EXPECT_FALSE(OffersDouble(MakeFiltered(metrics_sdk::ExemplarFilterType::kTraceBased),
                            ContextWithSpan(false)));
}

#endif  // ENABLE_METRICS_EXEMPLAR_PREVIEW
