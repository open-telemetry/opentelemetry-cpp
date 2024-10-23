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
#include "opentelemetry/metrics/meter.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace semconv
{
namespace http
{

/**
 * Duration of HTTP client requests.
 * <p>
 * histogram
 */
static constexpr const char *kMetricHttpClientRequestDuration =
    "metric.http.client.request.duration";
static constexpr const char *descrMetricHttpClientRequestDuration =
    "Duration of HTTP client requests.";
static constexpr const char *unitMetricHttpClientRequestDuration = "s";

static inline nostd::unique_ptr<metrics::Histogram<uint64_t>>
CreateSyncInt64MetricHttpClientRequestDuration(metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricHttpClientRequestDuration,
                                      descrMetricHttpClientRequestDuration,
                                      unitMetricHttpClientRequestDuration);
}

static inline nostd::unique_ptr<metrics::Histogram<double>>
CreateSyncDoubleMetricHttpClientRequestDuration(metrics::Meter *meter)
{
  return meter->CreateDoubleHistogram(kMetricHttpClientRequestDuration,
                                      descrMetricHttpClientRequestDuration,
                                      unitMetricHttpClientRequestDuration);
}

#ifdef OPENTELEMETRY_LATER
// Unsupported: Async histogram
static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncInt64MetricHttpClientRequestDuration(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableHistogram(kMetricHttpClientRequestDuration,
                                               descrMetricHttpClientRequestDuration,
                                               unitMetricHttpClientRequestDuration);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncDoubleMetricHttpClientRequestDuration(metrics::Meter *meter)
{
  return meter->CreateDoubleObservableHistogram(kMetricHttpClientRequestDuration,
                                                descrMetricHttpClientRequestDuration,
                                                unitMetricHttpClientRequestDuration);
}
#endif /* OPENTELEMETRY_LATER */

/**
 * Duration of HTTP server requests.
 * <p>
 * histogram
 */
static constexpr const char *kMetricHttpServerRequestDuration =
    "metric.http.server.request.duration";
static constexpr const char *descrMetricHttpServerRequestDuration =
    "Duration of HTTP server requests.";
static constexpr const char *unitMetricHttpServerRequestDuration = "s";

static inline nostd::unique_ptr<metrics::Histogram<uint64_t>>
CreateSyncInt64MetricHttpServerRequestDuration(metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricHttpServerRequestDuration,
                                      descrMetricHttpServerRequestDuration,
                                      unitMetricHttpServerRequestDuration);
}

static inline nostd::unique_ptr<metrics::Histogram<double>>
CreateSyncDoubleMetricHttpServerRequestDuration(metrics::Meter *meter)
{
  return meter->CreateDoubleHistogram(kMetricHttpServerRequestDuration,
                                      descrMetricHttpServerRequestDuration,
                                      unitMetricHttpServerRequestDuration);
}

#ifdef OPENTELEMETRY_LATER
// Unsupported: Async histogram
static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncInt64MetricHttpServerRequestDuration(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableHistogram(kMetricHttpServerRequestDuration,
                                               descrMetricHttpServerRequestDuration,
                                               unitMetricHttpServerRequestDuration);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncDoubleMetricHttpServerRequestDuration(metrics::Meter *meter)
{
  return meter->CreateDoubleObservableHistogram(kMetricHttpServerRequestDuration,
                                                descrMetricHttpServerRequestDuration,
                                                unitMetricHttpServerRequestDuration);
}
#endif /* OPENTELEMETRY_LATER */

}  // namespace http
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
