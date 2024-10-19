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

static nostd::unique_ptr<metrics::Histogram<uint64_t>> CreateSyncMetricHttpClientRequestDuration(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricHttpClientRequestDuration,
                                      descrMetricHttpClientRequestDuration,
                                      unitMetricHttpClientRequestDuration);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricHttpClientRequestDuration(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableHistogram(kMetricHttpClientRequestDuration,
                                               descrMetricHttpClientRequestDuration,
                                               unitMetricHttpClientRequestDuration);
}

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

static nostd::unique_ptr<metrics::Histogram<uint64_t>> CreateSyncMetricHttpServerRequestDuration(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricHttpServerRequestDuration,
                                      descrMetricHttpServerRequestDuration,
                                      unitMetricHttpServerRequestDuration);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricHttpServerRequestDuration(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableHistogram(kMetricHttpServerRequestDuration,
                                               descrMetricHttpServerRequestDuration,
                                               unitMetricHttpServerRequestDuration);
}

}  // namespace http
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
