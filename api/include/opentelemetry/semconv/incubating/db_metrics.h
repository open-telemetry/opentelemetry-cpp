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
namespace db
{

/**
 * The number of connections that are currently in state described by the @code state @endcode
 * attribute <p> updowncounter
 */
static constexpr const char *kMetricDbClientConnectionCount = "metric.db.client.connection.count";
static constexpr const char *descrMetricDbClientConnectionCount =
    "The number of connections that are currently in state described by the `state` attribute";
static constexpr const char *unitMetricDbClientConnectionCount = "{connection}";

static inline nostd::unique_ptr<metrics::UpDownCounter<int64_t>>
CreateSyncInt64MetricDbClientConnectionCount(metrics::Meter *meter)
{
  return meter->CreateInt64UpDownCounter(kMetricDbClientConnectionCount,
                                         descrMetricDbClientConnectionCount,
                                         unitMetricDbClientConnectionCount);
}

static inline nostd::unique_ptr<metrics::UpDownCounter<double>>
CreateSyncDoubleMetricDbClientConnectionCount(metrics::Meter *meter)
{
  return meter->CreateDoubleUpDownCounter(kMetricDbClientConnectionCount,
                                          descrMetricDbClientConnectionCount,
                                          unitMetricDbClientConnectionCount);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncInt64MetricDbClientConnectionCount(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableUpDownCounter(kMetricDbClientConnectionCount,
                                                   descrMetricDbClientConnectionCount,
                                                   unitMetricDbClientConnectionCount);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncDoubleMetricDbClientConnectionCount(metrics::Meter *meter)
{
  return meter->CreateDoubleObservableUpDownCounter(kMetricDbClientConnectionCount,
                                                    descrMetricDbClientConnectionCount,
                                                    unitMetricDbClientConnectionCount);
}

/**
 * The time it took to create a new connection
 * <p>
 * histogram
 */
static constexpr const char *kMetricDbClientConnectionCreateTime =
    "metric.db.client.connection.create_time";
static constexpr const char *descrMetricDbClientConnectionCreateTime =
    "The time it took to create a new connection";
static constexpr const char *unitMetricDbClientConnectionCreateTime = "s";

static inline nostd::unique_ptr<metrics::Histogram<uint64_t>>
CreateSyncInt64MetricDbClientConnectionCreateTime(metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricDbClientConnectionCreateTime,
                                      descrMetricDbClientConnectionCreateTime,
                                      unitMetricDbClientConnectionCreateTime);
}

static inline nostd::unique_ptr<metrics::Histogram<double>>
CreateSyncDoubleMetricDbClientConnectionCreateTime(metrics::Meter *meter)
{
  return meter->CreateDoubleHistogram(kMetricDbClientConnectionCreateTime,
                                      descrMetricDbClientConnectionCreateTime,
                                      unitMetricDbClientConnectionCreateTime);
}

/**
 * The maximum number of idle open connections allowed
 * <p>
 * updowncounter
 */
static constexpr const char *kMetricDbClientConnectionIdleMax =
    "metric.db.client.connection.idle.max";
static constexpr const char *descrMetricDbClientConnectionIdleMax =
    "The maximum number of idle open connections allowed";
static constexpr const char *unitMetricDbClientConnectionIdleMax = "{connection}";

static inline nostd::unique_ptr<metrics::UpDownCounter<int64_t>>
CreateSyncInt64MetricDbClientConnectionIdleMax(metrics::Meter *meter)
{
  return meter->CreateInt64UpDownCounter(kMetricDbClientConnectionIdleMax,
                                         descrMetricDbClientConnectionIdleMax,
                                         unitMetricDbClientConnectionIdleMax);
}

static inline nostd::unique_ptr<metrics::UpDownCounter<double>>
CreateSyncDoubleMetricDbClientConnectionIdleMax(metrics::Meter *meter)
{
  return meter->CreateDoubleUpDownCounter(kMetricDbClientConnectionIdleMax,
                                          descrMetricDbClientConnectionIdleMax,
                                          unitMetricDbClientConnectionIdleMax);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncInt64MetricDbClientConnectionIdleMax(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableUpDownCounter(kMetricDbClientConnectionIdleMax,
                                                   descrMetricDbClientConnectionIdleMax,
                                                   unitMetricDbClientConnectionIdleMax);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncDoubleMetricDbClientConnectionIdleMax(metrics::Meter *meter)
{
  return meter->CreateDoubleObservableUpDownCounter(kMetricDbClientConnectionIdleMax,
                                                    descrMetricDbClientConnectionIdleMax,
                                                    unitMetricDbClientConnectionIdleMax);
}

/**
 * The minimum number of idle open connections allowed
 * <p>
 * updowncounter
 */
static constexpr const char *kMetricDbClientConnectionIdleMin =
    "metric.db.client.connection.idle.min";
static constexpr const char *descrMetricDbClientConnectionIdleMin =
    "The minimum number of idle open connections allowed";
static constexpr const char *unitMetricDbClientConnectionIdleMin = "{connection}";

static inline nostd::unique_ptr<metrics::UpDownCounter<int64_t>>
CreateSyncInt64MetricDbClientConnectionIdleMin(metrics::Meter *meter)
{
  return meter->CreateInt64UpDownCounter(kMetricDbClientConnectionIdleMin,
                                         descrMetricDbClientConnectionIdleMin,
                                         unitMetricDbClientConnectionIdleMin);
}

static inline nostd::unique_ptr<metrics::UpDownCounter<double>>
CreateSyncDoubleMetricDbClientConnectionIdleMin(metrics::Meter *meter)
{
  return meter->CreateDoubleUpDownCounter(kMetricDbClientConnectionIdleMin,
                                          descrMetricDbClientConnectionIdleMin,
                                          unitMetricDbClientConnectionIdleMin);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncInt64MetricDbClientConnectionIdleMin(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableUpDownCounter(kMetricDbClientConnectionIdleMin,
                                                   descrMetricDbClientConnectionIdleMin,
                                                   unitMetricDbClientConnectionIdleMin);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncDoubleMetricDbClientConnectionIdleMin(metrics::Meter *meter)
{
  return meter->CreateDoubleObservableUpDownCounter(kMetricDbClientConnectionIdleMin,
                                                    descrMetricDbClientConnectionIdleMin,
                                                    unitMetricDbClientConnectionIdleMin);
}

/**
 * The maximum number of open connections allowed
 * <p>
 * updowncounter
 */
static constexpr const char *kMetricDbClientConnectionMax = "metric.db.client.connection.max";
static constexpr const char *descrMetricDbClientConnectionMax =
    "The maximum number of open connections allowed";
static constexpr const char *unitMetricDbClientConnectionMax = "{connection}";

static inline nostd::unique_ptr<metrics::UpDownCounter<int64_t>>
CreateSyncInt64MetricDbClientConnectionMax(metrics::Meter *meter)
{
  return meter->CreateInt64UpDownCounter(kMetricDbClientConnectionMax,
                                         descrMetricDbClientConnectionMax,
                                         unitMetricDbClientConnectionMax);
}

static inline nostd::unique_ptr<metrics::UpDownCounter<double>>
CreateSyncDoubleMetricDbClientConnectionMax(metrics::Meter *meter)
{
  return meter->CreateDoubleUpDownCounter(kMetricDbClientConnectionMax,
                                          descrMetricDbClientConnectionMax,
                                          unitMetricDbClientConnectionMax);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncInt64MetricDbClientConnectionMax(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableUpDownCounter(kMetricDbClientConnectionMax,
                                                   descrMetricDbClientConnectionMax,
                                                   unitMetricDbClientConnectionMax);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncDoubleMetricDbClientConnectionMax(metrics::Meter *meter)
{
  return meter->CreateDoubleObservableUpDownCounter(kMetricDbClientConnectionMax,
                                                    descrMetricDbClientConnectionMax,
                                                    unitMetricDbClientConnectionMax);
}

/**
 * The number of current pending requests for an open connection
 * <p>
 * updowncounter
 */
static constexpr const char *kMetricDbClientConnectionPendingRequests =
    "metric.db.client.connection.pending_requests";
static constexpr const char *descrMetricDbClientConnectionPendingRequests =
    "The number of current pending requests for an open connection";
static constexpr const char *unitMetricDbClientConnectionPendingRequests = "{request}";

static inline nostd::unique_ptr<metrics::UpDownCounter<int64_t>>
CreateSyncInt64MetricDbClientConnectionPendingRequests(metrics::Meter *meter)
{
  return meter->CreateInt64UpDownCounter(kMetricDbClientConnectionPendingRequests,
                                         descrMetricDbClientConnectionPendingRequests,
                                         unitMetricDbClientConnectionPendingRequests);
}

static inline nostd::unique_ptr<metrics::UpDownCounter<double>>
CreateSyncDoubleMetricDbClientConnectionPendingRequests(metrics::Meter *meter)
{
  return meter->CreateDoubleUpDownCounter(kMetricDbClientConnectionPendingRequests,
                                          descrMetricDbClientConnectionPendingRequests,
                                          unitMetricDbClientConnectionPendingRequests);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncInt64MetricDbClientConnectionPendingRequests(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableUpDownCounter(kMetricDbClientConnectionPendingRequests,
                                                   descrMetricDbClientConnectionPendingRequests,
                                                   unitMetricDbClientConnectionPendingRequests);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncDoubleMetricDbClientConnectionPendingRequests(metrics::Meter *meter)
{
  return meter->CreateDoubleObservableUpDownCounter(kMetricDbClientConnectionPendingRequests,
                                                    descrMetricDbClientConnectionPendingRequests,
                                                    unitMetricDbClientConnectionPendingRequests);
}

/**
 * The number of connection timeouts that have occurred trying to obtain a connection from the pool
 * <p>
 * counter
 */
static constexpr const char *kMetricDbClientConnectionTimeouts =
    "metric.db.client.connection.timeouts";
static constexpr const char *descrMetricDbClientConnectionTimeouts =
    "The number of connection timeouts that have occurred trying to obtain a connection from the "
    "pool";
static constexpr const char *unitMetricDbClientConnectionTimeouts = "{timeout}";

static inline nostd::unique_ptr<metrics::Counter<uint64_t>>
CreateSyncInt64MetricDbClientConnectionTimeouts(metrics::Meter *meter)
{
  return meter->CreateUInt64Counter(kMetricDbClientConnectionTimeouts,
                                    descrMetricDbClientConnectionTimeouts,
                                    unitMetricDbClientConnectionTimeouts);
}

static inline nostd::unique_ptr<metrics::Counter<double>>
CreateSyncDoubleMetricDbClientConnectionTimeouts(metrics::Meter *meter)
{
  return meter->CreateDoubleCounter(kMetricDbClientConnectionTimeouts,
                                    descrMetricDbClientConnectionTimeouts,
                                    unitMetricDbClientConnectionTimeouts);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncInt64MetricDbClientConnectionTimeouts(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableCounter(kMetricDbClientConnectionTimeouts,
                                             descrMetricDbClientConnectionTimeouts,
                                             unitMetricDbClientConnectionTimeouts);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncDoubleMetricDbClientConnectionTimeouts(metrics::Meter *meter)
{
  return meter->CreateDoubleObservableCounter(kMetricDbClientConnectionTimeouts,
                                              descrMetricDbClientConnectionTimeouts,
                                              unitMetricDbClientConnectionTimeouts);
}

/**
 * The time between borrowing a connection and returning it to the pool
 * <p>
 * histogram
 */
static constexpr const char *kMetricDbClientConnectionUseTime =
    "metric.db.client.connection.use_time";
static constexpr const char *descrMetricDbClientConnectionUseTime =
    "The time between borrowing a connection and returning it to the pool";
static constexpr const char *unitMetricDbClientConnectionUseTime = "s";

static inline nostd::unique_ptr<metrics::Histogram<uint64_t>>
CreateSyncInt64MetricDbClientConnectionUseTime(metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricDbClientConnectionUseTime,
                                      descrMetricDbClientConnectionUseTime,
                                      unitMetricDbClientConnectionUseTime);
}

static inline nostd::unique_ptr<metrics::Histogram<double>>
CreateSyncDoubleMetricDbClientConnectionUseTime(metrics::Meter *meter)
{
  return meter->CreateDoubleHistogram(kMetricDbClientConnectionUseTime,
                                      descrMetricDbClientConnectionUseTime,
                                      unitMetricDbClientConnectionUseTime);
}

/**
 * The time it took to obtain an open connection from the pool
 * <p>
 * histogram
 */
static constexpr const char *kMetricDbClientConnectionWaitTime =
    "metric.db.client.connection.wait_time";
static constexpr const char *descrMetricDbClientConnectionWaitTime =
    "The time it took to obtain an open connection from the pool";
static constexpr const char *unitMetricDbClientConnectionWaitTime = "s";

static inline nostd::unique_ptr<metrics::Histogram<uint64_t>>
CreateSyncInt64MetricDbClientConnectionWaitTime(metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricDbClientConnectionWaitTime,
                                      descrMetricDbClientConnectionWaitTime,
                                      unitMetricDbClientConnectionWaitTime);
}

static inline nostd::unique_ptr<metrics::Histogram<double>>
CreateSyncDoubleMetricDbClientConnectionWaitTime(metrics::Meter *meter)
{
  return meter->CreateDoubleHistogram(kMetricDbClientConnectionWaitTime,
                                      descrMetricDbClientConnectionWaitTime,
                                      unitMetricDbClientConnectionWaitTime);
}

/**
 * Deprecated, use @code db.client.connection.create_time @endcode instead. Note: the unit also
 * changed from @code ms @endcode to @code s @endcode. <p>
 * @deprecated
 * Replaced by @code db.client.connection.create_time @endcode. Note: the unit also changed from
 * @code ms @endcode to @code s @endcode. <p> histogram
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kMetricDbClientConnectionsCreateTimeDeprecated =
    "metric.db.client.connections.create_time.deprecated";
OPENTELEMETRY_DEPRECATED
static constexpr const char *descrMetricDbClientConnectionsCreateTimeDeprecated =
    "Deprecated, use `db.client.connection.create_time` instead. Note: the unit also changed from "
    "`ms` to `s`.";
OPENTELEMETRY_DEPRECATED
static constexpr const char *unitMetricDbClientConnectionsCreateTimeDeprecated = "ms";

OPENTELEMETRY_DEPRECATED
static inline nostd::unique_ptr<metrics::Histogram<uint64_t>>
CreateSyncInt64MetricDbClientConnectionsCreateTimeDeprecated(metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricDbClientConnectionsCreateTimeDeprecated,
                                      descrMetricDbClientConnectionsCreateTimeDeprecated,
                                      unitMetricDbClientConnectionsCreateTimeDeprecated);
}

OPENTELEMETRY_DEPRECATED
static inline nostd::unique_ptr<metrics::Histogram<double>>
CreateSyncDoubleMetricDbClientConnectionsCreateTimeDeprecated(metrics::Meter *meter)
{
  return meter->CreateDoubleHistogram(kMetricDbClientConnectionsCreateTimeDeprecated,
                                      descrMetricDbClientConnectionsCreateTimeDeprecated,
                                      unitMetricDbClientConnectionsCreateTimeDeprecated);
}

/**
 * Deprecated, use @code db.client.connection.idle.max @endcode instead.
 * <p>
 * @deprecated
 * Replaced by @code db.client.connection.idle.max @endcode.
 * <p>
 * updowncounter
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kMetricDbClientConnectionsIdleMaxDeprecated =
    "metric.db.client.connections.idle.max.deprecated";
OPENTELEMETRY_DEPRECATED
static constexpr const char *descrMetricDbClientConnectionsIdleMaxDeprecated =
    "Deprecated, use `db.client.connection.idle.max` instead.";
OPENTELEMETRY_DEPRECATED
static constexpr const char *unitMetricDbClientConnectionsIdleMaxDeprecated = "{connection}";

OPENTELEMETRY_DEPRECATED
static inline nostd::unique_ptr<metrics::UpDownCounter<int64_t>>
CreateSyncInt64MetricDbClientConnectionsIdleMaxDeprecated(metrics::Meter *meter)
{
  return meter->CreateInt64UpDownCounter(kMetricDbClientConnectionsIdleMaxDeprecated,
                                         descrMetricDbClientConnectionsIdleMaxDeprecated,
                                         unitMetricDbClientConnectionsIdleMaxDeprecated);
}

OPENTELEMETRY_DEPRECATED
static inline nostd::unique_ptr<metrics::UpDownCounter<double>>
CreateSyncDoubleMetricDbClientConnectionsIdleMaxDeprecated(metrics::Meter *meter)
{
  return meter->CreateDoubleUpDownCounter(kMetricDbClientConnectionsIdleMaxDeprecated,
                                          descrMetricDbClientConnectionsIdleMaxDeprecated,
                                          unitMetricDbClientConnectionsIdleMaxDeprecated);
}

OPENTELEMETRY_DEPRECATED
static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncInt64MetricDbClientConnectionsIdleMaxDeprecated(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableUpDownCounter(kMetricDbClientConnectionsIdleMaxDeprecated,
                                                   descrMetricDbClientConnectionsIdleMaxDeprecated,
                                                   unitMetricDbClientConnectionsIdleMaxDeprecated);
}

OPENTELEMETRY_DEPRECATED
static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncDoubleMetricDbClientConnectionsIdleMaxDeprecated(metrics::Meter *meter)
{
  return meter->CreateDoubleObservableUpDownCounter(kMetricDbClientConnectionsIdleMaxDeprecated,
                                                    descrMetricDbClientConnectionsIdleMaxDeprecated,
                                                    unitMetricDbClientConnectionsIdleMaxDeprecated);
}

/**
 * Deprecated, use @code db.client.connection.idle.min @endcode instead.
 * <p>
 * @deprecated
 * Replaced by @code db.client.connection.idle.min @endcode.
 * <p>
 * updowncounter
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kMetricDbClientConnectionsIdleMinDeprecated =
    "metric.db.client.connections.idle.min.deprecated";
OPENTELEMETRY_DEPRECATED
static constexpr const char *descrMetricDbClientConnectionsIdleMinDeprecated =
    "Deprecated, use `db.client.connection.idle.min` instead.";
OPENTELEMETRY_DEPRECATED
static constexpr const char *unitMetricDbClientConnectionsIdleMinDeprecated = "{connection}";

OPENTELEMETRY_DEPRECATED
static inline nostd::unique_ptr<metrics::UpDownCounter<int64_t>>
CreateSyncInt64MetricDbClientConnectionsIdleMinDeprecated(metrics::Meter *meter)
{
  return meter->CreateInt64UpDownCounter(kMetricDbClientConnectionsIdleMinDeprecated,
                                         descrMetricDbClientConnectionsIdleMinDeprecated,
                                         unitMetricDbClientConnectionsIdleMinDeprecated);
}

OPENTELEMETRY_DEPRECATED
static inline nostd::unique_ptr<metrics::UpDownCounter<double>>
CreateSyncDoubleMetricDbClientConnectionsIdleMinDeprecated(metrics::Meter *meter)
{
  return meter->CreateDoubleUpDownCounter(kMetricDbClientConnectionsIdleMinDeprecated,
                                          descrMetricDbClientConnectionsIdleMinDeprecated,
                                          unitMetricDbClientConnectionsIdleMinDeprecated);
}

OPENTELEMETRY_DEPRECATED
static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncInt64MetricDbClientConnectionsIdleMinDeprecated(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableUpDownCounter(kMetricDbClientConnectionsIdleMinDeprecated,
                                                   descrMetricDbClientConnectionsIdleMinDeprecated,
                                                   unitMetricDbClientConnectionsIdleMinDeprecated);
}

OPENTELEMETRY_DEPRECATED
static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncDoubleMetricDbClientConnectionsIdleMinDeprecated(metrics::Meter *meter)
{
  return meter->CreateDoubleObservableUpDownCounter(kMetricDbClientConnectionsIdleMinDeprecated,
                                                    descrMetricDbClientConnectionsIdleMinDeprecated,
                                                    unitMetricDbClientConnectionsIdleMinDeprecated);
}

/**
 * Deprecated, use @code db.client.connection.max @endcode instead.
 * <p>
 * @deprecated
 * Replaced by @code db.client.connection.max @endcode.
 * <p>
 * updowncounter
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kMetricDbClientConnectionsMaxDeprecated =
    "metric.db.client.connections.max.deprecated";
OPENTELEMETRY_DEPRECATED
static constexpr const char *descrMetricDbClientConnectionsMaxDeprecated =
    "Deprecated, use `db.client.connection.max` instead.";
OPENTELEMETRY_DEPRECATED
static constexpr const char *unitMetricDbClientConnectionsMaxDeprecated = "{connection}";

OPENTELEMETRY_DEPRECATED
static inline nostd::unique_ptr<metrics::UpDownCounter<int64_t>>
CreateSyncInt64MetricDbClientConnectionsMaxDeprecated(metrics::Meter *meter)
{
  return meter->CreateInt64UpDownCounter(kMetricDbClientConnectionsMaxDeprecated,
                                         descrMetricDbClientConnectionsMaxDeprecated,
                                         unitMetricDbClientConnectionsMaxDeprecated);
}

OPENTELEMETRY_DEPRECATED
static inline nostd::unique_ptr<metrics::UpDownCounter<double>>
CreateSyncDoubleMetricDbClientConnectionsMaxDeprecated(metrics::Meter *meter)
{
  return meter->CreateDoubleUpDownCounter(kMetricDbClientConnectionsMaxDeprecated,
                                          descrMetricDbClientConnectionsMaxDeprecated,
                                          unitMetricDbClientConnectionsMaxDeprecated);
}

OPENTELEMETRY_DEPRECATED
static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncInt64MetricDbClientConnectionsMaxDeprecated(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableUpDownCounter(kMetricDbClientConnectionsMaxDeprecated,
                                                   descrMetricDbClientConnectionsMaxDeprecated,
                                                   unitMetricDbClientConnectionsMaxDeprecated);
}

OPENTELEMETRY_DEPRECATED
static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncDoubleMetricDbClientConnectionsMaxDeprecated(metrics::Meter *meter)
{
  return meter->CreateDoubleObservableUpDownCounter(kMetricDbClientConnectionsMaxDeprecated,
                                                    descrMetricDbClientConnectionsMaxDeprecated,
                                                    unitMetricDbClientConnectionsMaxDeprecated);
}

/**
 * Deprecated, use @code db.client.connection.pending_requests @endcode instead.
 * <p>
 * @deprecated
 * Replaced by @code db.client.connection.pending_requests @endcode.
 * <p>
 * updowncounter
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kMetricDbClientConnectionsPendingRequestsDeprecated =
    "metric.db.client.connections.pending_requests.deprecated";
OPENTELEMETRY_DEPRECATED
static constexpr const char *descrMetricDbClientConnectionsPendingRequestsDeprecated =
    "Deprecated, use `db.client.connection.pending_requests` instead.";
OPENTELEMETRY_DEPRECATED
static constexpr const char *unitMetricDbClientConnectionsPendingRequestsDeprecated = "{request}";

OPENTELEMETRY_DEPRECATED
static inline nostd::unique_ptr<metrics::UpDownCounter<int64_t>>
CreateSyncInt64MetricDbClientConnectionsPendingRequestsDeprecated(metrics::Meter *meter)
{
  return meter->CreateInt64UpDownCounter(kMetricDbClientConnectionsPendingRequestsDeprecated,
                                         descrMetricDbClientConnectionsPendingRequestsDeprecated,
                                         unitMetricDbClientConnectionsPendingRequestsDeprecated);
}

OPENTELEMETRY_DEPRECATED
static inline nostd::unique_ptr<metrics::UpDownCounter<double>>
CreateSyncDoubleMetricDbClientConnectionsPendingRequestsDeprecated(metrics::Meter *meter)
{
  return meter->CreateDoubleUpDownCounter(kMetricDbClientConnectionsPendingRequestsDeprecated,
                                          descrMetricDbClientConnectionsPendingRequestsDeprecated,
                                          unitMetricDbClientConnectionsPendingRequestsDeprecated);
}

OPENTELEMETRY_DEPRECATED
static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncInt64MetricDbClientConnectionsPendingRequestsDeprecated(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableUpDownCounter(
      kMetricDbClientConnectionsPendingRequestsDeprecated,
      descrMetricDbClientConnectionsPendingRequestsDeprecated,
      unitMetricDbClientConnectionsPendingRequestsDeprecated);
}

OPENTELEMETRY_DEPRECATED
static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncDoubleMetricDbClientConnectionsPendingRequestsDeprecated(metrics::Meter *meter)
{
  return meter->CreateDoubleObservableUpDownCounter(
      kMetricDbClientConnectionsPendingRequestsDeprecated,
      descrMetricDbClientConnectionsPendingRequestsDeprecated,
      unitMetricDbClientConnectionsPendingRequestsDeprecated);
}

/**
 * Deprecated, use @code db.client.connection.timeouts @endcode instead.
 * <p>
 * @deprecated
 * Replaced by @code db.client.connection.timeouts @endcode.
 * <p>
 * counter
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kMetricDbClientConnectionsTimeoutsDeprecated =
    "metric.db.client.connections.timeouts.deprecated";
OPENTELEMETRY_DEPRECATED
static constexpr const char *descrMetricDbClientConnectionsTimeoutsDeprecated =
    "Deprecated, use `db.client.connection.timeouts` instead.";
OPENTELEMETRY_DEPRECATED
static constexpr const char *unitMetricDbClientConnectionsTimeoutsDeprecated = "{timeout}";

OPENTELEMETRY_DEPRECATED
static inline nostd::unique_ptr<metrics::Counter<uint64_t>>
CreateSyncInt64MetricDbClientConnectionsTimeoutsDeprecated(metrics::Meter *meter)
{
  return meter->CreateUInt64Counter(kMetricDbClientConnectionsTimeoutsDeprecated,
                                    descrMetricDbClientConnectionsTimeoutsDeprecated,
                                    unitMetricDbClientConnectionsTimeoutsDeprecated);
}

OPENTELEMETRY_DEPRECATED
static inline nostd::unique_ptr<metrics::Counter<double>>
CreateSyncDoubleMetricDbClientConnectionsTimeoutsDeprecated(metrics::Meter *meter)
{
  return meter->CreateDoubleCounter(kMetricDbClientConnectionsTimeoutsDeprecated,
                                    descrMetricDbClientConnectionsTimeoutsDeprecated,
                                    unitMetricDbClientConnectionsTimeoutsDeprecated);
}

OPENTELEMETRY_DEPRECATED
static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncInt64MetricDbClientConnectionsTimeoutsDeprecated(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableCounter(kMetricDbClientConnectionsTimeoutsDeprecated,
                                             descrMetricDbClientConnectionsTimeoutsDeprecated,
                                             unitMetricDbClientConnectionsTimeoutsDeprecated);
}

OPENTELEMETRY_DEPRECATED
static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncDoubleMetricDbClientConnectionsTimeoutsDeprecated(metrics::Meter *meter)
{
  return meter->CreateDoubleObservableCounter(kMetricDbClientConnectionsTimeoutsDeprecated,
                                              descrMetricDbClientConnectionsTimeoutsDeprecated,
                                              unitMetricDbClientConnectionsTimeoutsDeprecated);
}

/**
 * Deprecated, use @code db.client.connection.count @endcode instead.
 * <p>
 * @deprecated
 * Replaced by @code db.client.connection.count @endcode.
 * <p>
 * updowncounter
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kMetricDbClientConnectionsCountDeprecated =
    "metric.db.client.connections.count.deprecated";
OPENTELEMETRY_DEPRECATED
static constexpr const char *descrMetricDbClientConnectionsCountDeprecated =
    "Deprecated, use `db.client.connection.count` instead.";
OPENTELEMETRY_DEPRECATED
static constexpr const char *unitMetricDbClientConnectionsCountDeprecated = "{connection}";

OPENTELEMETRY_DEPRECATED
static inline nostd::unique_ptr<metrics::UpDownCounter<int64_t>>
CreateSyncInt64MetricDbClientConnectionsCountDeprecated(metrics::Meter *meter)
{
  return meter->CreateInt64UpDownCounter(kMetricDbClientConnectionsCountDeprecated,
                                         descrMetricDbClientConnectionsCountDeprecated,
                                         unitMetricDbClientConnectionsCountDeprecated);
}

OPENTELEMETRY_DEPRECATED
static inline nostd::unique_ptr<metrics::UpDownCounter<double>>
CreateSyncDoubleMetricDbClientConnectionsCountDeprecated(metrics::Meter *meter)
{
  return meter->CreateDoubleUpDownCounter(kMetricDbClientConnectionsCountDeprecated,
                                          descrMetricDbClientConnectionsCountDeprecated,
                                          unitMetricDbClientConnectionsCountDeprecated);
}

OPENTELEMETRY_DEPRECATED
static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncInt64MetricDbClientConnectionsCountDeprecated(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableUpDownCounter(kMetricDbClientConnectionsCountDeprecated,
                                                   descrMetricDbClientConnectionsCountDeprecated,
                                                   unitMetricDbClientConnectionsCountDeprecated);
}

OPENTELEMETRY_DEPRECATED
static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncDoubleMetricDbClientConnectionsCountDeprecated(metrics::Meter *meter)
{
  return meter->CreateDoubleObservableUpDownCounter(kMetricDbClientConnectionsCountDeprecated,
                                                    descrMetricDbClientConnectionsCountDeprecated,
                                                    unitMetricDbClientConnectionsCountDeprecated);
}

/**
 * Deprecated, use @code db.client.connection.use_time @endcode instead. Note: the unit also changed
 * from @code ms @endcode to @code s @endcode. <p>
 * @deprecated
 * Replaced by @code db.client.connection.use_time @endcode. Note: the unit also changed from @code
 * ms @endcode to @code s @endcode. <p> histogram
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kMetricDbClientConnectionsUseTimeDeprecated =
    "metric.db.client.connections.use_time.deprecated";
OPENTELEMETRY_DEPRECATED
static constexpr const char *descrMetricDbClientConnectionsUseTimeDeprecated =
    "Deprecated, use `db.client.connection.use_time` instead. Note: the unit also changed from "
    "`ms` to `s`.";
OPENTELEMETRY_DEPRECATED
static constexpr const char *unitMetricDbClientConnectionsUseTimeDeprecated = "ms";

OPENTELEMETRY_DEPRECATED
static inline nostd::unique_ptr<metrics::Histogram<uint64_t>>
CreateSyncInt64MetricDbClientConnectionsUseTimeDeprecated(metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricDbClientConnectionsUseTimeDeprecated,
                                      descrMetricDbClientConnectionsUseTimeDeprecated,
                                      unitMetricDbClientConnectionsUseTimeDeprecated);
}

OPENTELEMETRY_DEPRECATED
static inline nostd::unique_ptr<metrics::Histogram<double>>
CreateSyncDoubleMetricDbClientConnectionsUseTimeDeprecated(metrics::Meter *meter)
{
  return meter->CreateDoubleHistogram(kMetricDbClientConnectionsUseTimeDeprecated,
                                      descrMetricDbClientConnectionsUseTimeDeprecated,
                                      unitMetricDbClientConnectionsUseTimeDeprecated);
}

/**
 * Deprecated, use @code db.client.connection.wait_time @endcode instead. Note: the unit also
 * changed from @code ms @endcode to @code s @endcode. <p>
 * @deprecated
 * Replaced by @code db.client.connection.wait_time @endcode. Note: the unit also changed from @code
 * ms @endcode to @code s @endcode. <p> histogram
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kMetricDbClientConnectionsWaitTimeDeprecated =
    "metric.db.client.connections.wait_time.deprecated";
OPENTELEMETRY_DEPRECATED
static constexpr const char *descrMetricDbClientConnectionsWaitTimeDeprecated =
    "Deprecated, use `db.client.connection.wait_time` instead. Note: the unit also changed from "
    "`ms` to `s`.";
OPENTELEMETRY_DEPRECATED
static constexpr const char *unitMetricDbClientConnectionsWaitTimeDeprecated = "ms";

OPENTELEMETRY_DEPRECATED
static inline nostd::unique_ptr<metrics::Histogram<uint64_t>>
CreateSyncInt64MetricDbClientConnectionsWaitTimeDeprecated(metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricDbClientConnectionsWaitTimeDeprecated,
                                      descrMetricDbClientConnectionsWaitTimeDeprecated,
                                      unitMetricDbClientConnectionsWaitTimeDeprecated);
}

OPENTELEMETRY_DEPRECATED
static inline nostd::unique_ptr<metrics::Histogram<double>>
CreateSyncDoubleMetricDbClientConnectionsWaitTimeDeprecated(metrics::Meter *meter)
{
  return meter->CreateDoubleHistogram(kMetricDbClientConnectionsWaitTimeDeprecated,
                                      descrMetricDbClientConnectionsWaitTimeDeprecated,
                                      unitMetricDbClientConnectionsWaitTimeDeprecated);
}

/**
 * Duration of database client operations.
 * <p>
 * Batch operations SHOULD be recorded as a single operation.
 * <p>
 * histogram
 */
static constexpr const char *kMetricDbClientOperationDuration =
    "metric.db.client.operation.duration";
static constexpr const char *descrMetricDbClientOperationDuration =
    "Duration of database client operations.";
static constexpr const char *unitMetricDbClientOperationDuration = "s";

static inline nostd::unique_ptr<metrics::Histogram<uint64_t>>
CreateSyncInt64MetricDbClientOperationDuration(metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricDbClientOperationDuration,
                                      descrMetricDbClientOperationDuration,
                                      unitMetricDbClientOperationDuration);
}

static inline nostd::unique_ptr<metrics::Histogram<double>>
CreateSyncDoubleMetricDbClientOperationDuration(metrics::Meter *meter)
{
  return meter->CreateDoubleHistogram(kMetricDbClientOperationDuration,
                                      descrMetricDbClientOperationDuration,
                                      unitMetricDbClientOperationDuration);
}

}  // namespace db
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
