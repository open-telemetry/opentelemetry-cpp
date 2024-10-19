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
 * Number of active HTTP requests.
 * <p>
 * updowncounter
 */
static constexpr const char *kMetricHttpClientActiveRequests = "metric.http.client.active_requests";
static constexpr const char *descrMetricHttpClientActiveRequests =
    "Number of active HTTP requests.";
static constexpr const char *unitMetricHttpClientActiveRequests = "{request}";

static nostd::unique_ptr<metrics::UpDownCounter<uint64_t>> CreateSyncMetricHttpClientActiveRequests(
    metrics::Meter *meter)
{
  return meter->CreateInt64UpDownCounter(kMetricHttpClientActiveRequests,
                                         descrMetricHttpClientActiveRequests,
                                         unitMetricHttpClientActiveRequests);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricHttpClientActiveRequests(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableUpDownCounter(kMetricHttpClientActiveRequests,
                                                   descrMetricHttpClientActiveRequests,
                                                   unitMetricHttpClientActiveRequests);
}

/**
 * The duration of the successfully established outbound HTTP connections.
 * <p>
 * histogram
 */
static constexpr const char *kMetricHttpClientConnectionDuration =
    "metric.http.client.connection.duration";
static constexpr const char *descrMetricHttpClientConnectionDuration =
    "The duration of the successfully established outbound HTTP connections.";
static constexpr const char *unitMetricHttpClientConnectionDuration = "s";

static nostd::unique_ptr<metrics::Histogram<uint64_t>> CreateSyncMetricHttpClientConnectionDuration(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricHttpClientConnectionDuration,
                                      descrMetricHttpClientConnectionDuration,
                                      unitMetricHttpClientConnectionDuration);
}

static nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncMetricHttpClientConnectionDuration(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableHistogram(kMetricHttpClientConnectionDuration,
                                               descrMetricHttpClientConnectionDuration,
                                               unitMetricHttpClientConnectionDuration);
}

/**
 * Number of outbound HTTP connections that are currently active or idle on the client.
 * <p>
 * updowncounter
 */
static constexpr const char *kMetricHttpClientOpenConnections =
    "metric.http.client.open_connections";
static constexpr const char *descrMetricHttpClientOpenConnections =
    "Number of outbound HTTP connections that are currently active or idle on the client.";
static constexpr const char *unitMetricHttpClientOpenConnections = "{connection}";

static nostd::unique_ptr<metrics::UpDownCounter<uint64_t>>
CreateSyncMetricHttpClientOpenConnections(metrics::Meter *meter)
{
  return meter->CreateInt64UpDownCounter(kMetricHttpClientOpenConnections,
                                         descrMetricHttpClientOpenConnections,
                                         unitMetricHttpClientOpenConnections);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricHttpClientOpenConnections(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableUpDownCounter(kMetricHttpClientOpenConnections,
                                                   descrMetricHttpClientOpenConnections,
                                                   unitMetricHttpClientOpenConnections);
}

/**
 * Size of HTTP client request bodies.
 * <p>
 * The size of the request payload body in bytes. This is the number of bytes transferred excluding
 * headers and is often, but not always, present as the <a
 * href="https://www.rfc-editor.org/rfc/rfc9110.html#field.content-length">Content-Length</a>
 * header. For requests using transport encoding, this should be the compressed size. <p> histogram
 */
static constexpr const char *kMetricHttpClientRequestBodySize =
    "metric.http.client.request.body.size";
static constexpr const char *descrMetricHttpClientRequestBodySize =
    "Size of HTTP client request bodies.";
static constexpr const char *unitMetricHttpClientRequestBodySize = "By";

static nostd::unique_ptr<metrics::Histogram<uint64_t>> CreateSyncMetricHttpClientRequestBodySize(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricHttpClientRequestBodySize,
                                      descrMetricHttpClientRequestBodySize,
                                      unitMetricHttpClientRequestBodySize);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricHttpClientRequestBodySize(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableHistogram(kMetricHttpClientRequestBodySize,
                                               descrMetricHttpClientRequestBodySize,
                                               unitMetricHttpClientRequestBodySize);
}

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
 * Size of HTTP client response bodies.
 * <p>
 * The size of the response payload body in bytes. This is the number of bytes transferred excluding
 * headers and is often, but not always, present as the <a
 * href="https://www.rfc-editor.org/rfc/rfc9110.html#field.content-length">Content-Length</a>
 * header. For requests using transport encoding, this should be the compressed size. <p> histogram
 */
static constexpr const char *kMetricHttpClientResponseBodySize =
    "metric.http.client.response.body.size";
static constexpr const char *descrMetricHttpClientResponseBodySize =
    "Size of HTTP client response bodies.";
static constexpr const char *unitMetricHttpClientResponseBodySize = "By";

static nostd::unique_ptr<metrics::Histogram<uint64_t>> CreateSyncMetricHttpClientResponseBodySize(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricHttpClientResponseBodySize,
                                      descrMetricHttpClientResponseBodySize,
                                      unitMetricHttpClientResponseBodySize);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricHttpClientResponseBodySize(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableHistogram(kMetricHttpClientResponseBodySize,
                                               descrMetricHttpClientResponseBodySize,
                                               unitMetricHttpClientResponseBodySize);
}

/**
 * Number of active HTTP server requests.
 * <p>
 * updowncounter
 */
static constexpr const char *kMetricHttpServerActiveRequests = "metric.http.server.active_requests";
static constexpr const char *descrMetricHttpServerActiveRequests =
    "Number of active HTTP server requests.";
static constexpr const char *unitMetricHttpServerActiveRequests = "{request}";

static nostd::unique_ptr<metrics::UpDownCounter<uint64_t>> CreateSyncMetricHttpServerActiveRequests(
    metrics::Meter *meter)
{
  return meter->CreateInt64UpDownCounter(kMetricHttpServerActiveRequests,
                                         descrMetricHttpServerActiveRequests,
                                         unitMetricHttpServerActiveRequests);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricHttpServerActiveRequests(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableUpDownCounter(kMetricHttpServerActiveRequests,
                                                   descrMetricHttpServerActiveRequests,
                                                   unitMetricHttpServerActiveRequests);
}

/**
 * Size of HTTP server request bodies.
 * <p>
 * The size of the request payload body in bytes. This is the number of bytes transferred excluding
 * headers and is often, but not always, present as the <a
 * href="https://www.rfc-editor.org/rfc/rfc9110.html#field.content-length">Content-Length</a>
 * header. For requests using transport encoding, this should be the compressed size. <p> histogram
 */
static constexpr const char *kMetricHttpServerRequestBodySize =
    "metric.http.server.request.body.size";
static constexpr const char *descrMetricHttpServerRequestBodySize =
    "Size of HTTP server request bodies.";
static constexpr const char *unitMetricHttpServerRequestBodySize = "By";

static nostd::unique_ptr<metrics::Histogram<uint64_t>> CreateSyncMetricHttpServerRequestBodySize(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricHttpServerRequestBodySize,
                                      descrMetricHttpServerRequestBodySize,
                                      unitMetricHttpServerRequestBodySize);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricHttpServerRequestBodySize(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableHistogram(kMetricHttpServerRequestBodySize,
                                               descrMetricHttpServerRequestBodySize,
                                               unitMetricHttpServerRequestBodySize);
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

/**
 * Size of HTTP server response bodies.
 * <p>
 * The size of the response payload body in bytes. This is the number of bytes transferred excluding
 * headers and is often, but not always, present as the <a
 * href="https://www.rfc-editor.org/rfc/rfc9110.html#field.content-length">Content-Length</a>
 * header. For requests using transport encoding, this should be the compressed size. <p> histogram
 */
static constexpr const char *kMetricHttpServerResponseBodySize =
    "metric.http.server.response.body.size";
static constexpr const char *descrMetricHttpServerResponseBodySize =
    "Size of HTTP server response bodies.";
static constexpr const char *unitMetricHttpServerResponseBodySize = "By";

static nostd::unique_ptr<metrics::Histogram<uint64_t>> CreateSyncMetricHttpServerResponseBodySize(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricHttpServerResponseBodySize,
                                      descrMetricHttpServerResponseBodySize,
                                      unitMetricHttpServerResponseBodySize);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricHttpServerResponseBodySize(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableHistogram(kMetricHttpServerResponseBodySize,
                                               descrMetricHttpServerResponseBodySize,
                                               unitMetricHttpServerResponseBodySize);
}

}  // namespace http
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
