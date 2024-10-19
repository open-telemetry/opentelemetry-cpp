/*
 * Copyright The OpenTelemetry Authors
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * DO NOT EDIT, this is an Auto-generated file from:
 * buildscripts/semantic-convention/templates/registry/semantic_metrics-h.j2
 */

#pragma once

#include "opentelemetry/common/macros.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace semconv
{
namespace dns
{

/**
 * Measures the time taken to perform a DNS lookup.
 * <p>
 * histogram
 */
static constexpr const char *kMetricDnsLookupDuration = "metric.dns.lookup.duration";
static constexpr const char *descrMetricDnsLookupDuration =
    "Measures the time taken to perform a DNS lookup.";
static constexpr const char *unitMetricDnsLookupDuration = "s";

static nostd::unique_ptr<metrics::Histogram<uint64_t>> CreateSyncMetricDnsLookupDuration(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricDnsLookupDuration, descrMetricDnsLookupDuration,
                                      unitMetricDnsLookupDuration);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricDnsLookupDuration(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableHistogram(
      kMetricDnsLookupDuration, descrMetricDnsLookupDuration, unitMetricDnsLookupDuration);
}

}  // namespace dns
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
