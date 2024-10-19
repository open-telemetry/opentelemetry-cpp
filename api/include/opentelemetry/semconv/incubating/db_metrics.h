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

static nostd::unique_ptr<metrics::UpDownCounter<uint64_t>> CreateSyncMetricDbClientConnectionCount(
    metrics::Meter *meter)
{
  return meter->CreateInt64UpDownCounter(kMetricDbClientConnectionCount,
                                         descrMetricDbClientConnectionCount,
                                         unitMetricDbClientConnectionCount);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricDbClientConnectionCount(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableUpDownCounter(kMetricDbClientConnectionCount,
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

static nostd::unique_ptr<metrics::Histogram<uint64_t>> CreateSyncMetricDbClientConnectionCreateTime(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricDbClientConnectionCreateTime,
                                      descrMetricDbClientConnectionCreateTime,
                                      unitMetricDbClientConnectionCreateTime);
}

static nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncMetricDbClientConnectionCreateTime(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableHistogram(kMetricDbClientConnectionCreateTime,
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

static nostd::unique_ptr<metrics::UpDownCounter<uint64_t>>
CreateSyncMetricDbClientConnectionIdleMax(metrics::Meter *meter)
{
  return meter->CreateInt64UpDownCounter(kMetricDbClientConnectionIdleMax,
                                         descrMetricDbClientConnectionIdleMax,
                                         unitMetricDbClientConnectionIdleMax);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricDbClientConnectionIdleMax(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableUpDownCounter(kMetricDbClientConnectionIdleMax,
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

static nostd::unique_ptr<metrics::UpDownCounter<uint64_t>>
CreateSyncMetricDbClientConnectionIdleMin(metrics::Meter *meter)
{
  return meter->CreateInt64UpDownCounter(kMetricDbClientConnectionIdleMin,
                                         descrMetricDbClientConnectionIdleMin,
                                         unitMetricDbClientConnectionIdleMin);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricDbClientConnectionIdleMin(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableUpDownCounter(kMetricDbClientConnectionIdleMin,
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

static nostd::unique_ptr<metrics::UpDownCounter<uint64_t>> CreateSyncMetricDbClientConnectionMax(
    metrics::Meter *meter)
{
  return meter->CreateInt64UpDownCounter(kMetricDbClientConnectionMax,
                                         descrMetricDbClientConnectionMax,
                                         unitMetricDbClientConnectionMax);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricDbClientConnectionMax(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableUpDownCounter(kMetricDbClientConnectionMax,
                                                   descrMetricDbClientConnectionMax,
                                                   unitMetricDbClientConnectionMax);
}

/**
 * The number of pending requests for an open connection, cumulative for the entire pool
 * <p>
 * updowncounter
 */
static constexpr const char *kMetricDbClientConnectionPendingRequests =
    "metric.db.client.connection.pending_requests";
static constexpr const char *descrMetricDbClientConnectionPendingRequests =
    "The number of pending requests for an open connection, cumulative for the entire pool";
static constexpr const char *unitMetricDbClientConnectionPendingRequests = "{request}";

static nostd::unique_ptr<metrics::UpDownCounter<uint64_t>>
CreateSyncMetricDbClientConnectionPendingRequests(metrics::Meter *meter)
{
  return meter->CreateInt64UpDownCounter(kMetricDbClientConnectionPendingRequests,
                                         descrMetricDbClientConnectionPendingRequests,
                                         unitMetricDbClientConnectionPendingRequests);
}

static nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncMetricDbClientConnectionPendingRequests(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableUpDownCounter(kMetricDbClientConnectionPendingRequests,
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

static nostd::unique_ptr<metrics::Counter<uint64_t>> CreateSyncMetricDbClientConnectionTimeouts(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Counter(kMetricDbClientConnectionTimeouts,
                                    descrMetricDbClientConnectionTimeouts,
                                    unitMetricDbClientConnectionTimeouts);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricDbClientConnectionTimeouts(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableCounter(kMetricDbClientConnectionTimeouts,
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

static nostd::unique_ptr<metrics::Histogram<uint64_t>> CreateSyncMetricDbClientConnectionUseTime(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricDbClientConnectionUseTime,
                                      descrMetricDbClientConnectionUseTime,
                                      unitMetricDbClientConnectionUseTime);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricDbClientConnectionUseTime(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableHistogram(kMetricDbClientConnectionUseTime,
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

static nostd::unique_ptr<metrics::Histogram<uint64_t>> CreateSyncMetricDbClientConnectionWaitTime(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricDbClientConnectionWaitTime,
                                      descrMetricDbClientConnectionWaitTime,
                                      unitMetricDbClientConnectionWaitTime);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricDbClientConnectionWaitTime(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableHistogram(kMetricDbClientConnectionWaitTime,
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
static constexpr const char *descrMetricDbClientConnectionsCreateTimeDeprecated =
    "Deprecated, use `db.client.connection.create_time` instead. Note: the unit also changed from "
    "`ms` to `s`.";
static constexpr const char *unitMetricDbClientConnectionsCreateTimeDeprecated = "ms";

static nostd::unique_ptr<metrics::Histogram<uint64_t>>
CreateSyncMetricDbClientConnectionsCreateTimeDeprecated(metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricDbClientConnectionsCreateTimeDeprecated,
                                      descrMetricDbClientConnectionsCreateTimeDeprecated,
                                      unitMetricDbClientConnectionsCreateTimeDeprecated);
}

static nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncMetricDbClientConnectionsCreateTimeDeprecated(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableHistogram(kMetricDbClientConnectionsCreateTimeDeprecated,
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
static constexpr const char *descrMetricDbClientConnectionsIdleMaxDeprecated =
    "Deprecated, use `db.client.connection.idle.max` instead.";
static constexpr const char *unitMetricDbClientConnectionsIdleMaxDeprecated = "{connection}";

static nostd::unique_ptr<metrics::UpDownCounter<uint64_t>>
CreateSyncMetricDbClientConnectionsIdleMaxDeprecated(metrics::Meter *meter)
{
  return meter->CreateInt64UpDownCounter(kMetricDbClientConnectionsIdleMaxDeprecated,
                                         descrMetricDbClientConnectionsIdleMaxDeprecated,
                                         unitMetricDbClientConnectionsIdleMaxDeprecated);
}

static nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncMetricDbClientConnectionsIdleMaxDeprecated(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableUpDownCounter(kMetricDbClientConnectionsIdleMaxDeprecated,
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
static constexpr const char *descrMetricDbClientConnectionsIdleMinDeprecated =
    "Deprecated, use `db.client.connection.idle.min` instead.";
static constexpr const char *unitMetricDbClientConnectionsIdleMinDeprecated = "{connection}";

static nostd::unique_ptr<metrics::UpDownCounter<uint64_t>>
CreateSyncMetricDbClientConnectionsIdleMinDeprecated(metrics::Meter *meter)
{
  return meter->CreateInt64UpDownCounter(kMetricDbClientConnectionsIdleMinDeprecated,
                                         descrMetricDbClientConnectionsIdleMinDeprecated,
                                         unitMetricDbClientConnectionsIdleMinDeprecated);
}

static nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncMetricDbClientConnectionsIdleMinDeprecated(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableUpDownCounter(kMetricDbClientConnectionsIdleMinDeprecated,
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
static constexpr const char *descrMetricDbClientConnectionsMaxDeprecated =
    "Deprecated, use `db.client.connection.max` instead.";
static constexpr const char *unitMetricDbClientConnectionsMaxDeprecated = "{connection}";

static nostd::unique_ptr<metrics::UpDownCounter<uint64_t>>
CreateSyncMetricDbClientConnectionsMaxDeprecated(metrics::Meter *meter)
{
  return meter->CreateInt64UpDownCounter(kMetricDbClientConnectionsMaxDeprecated,
                                         descrMetricDbClientConnectionsMaxDeprecated,
                                         unitMetricDbClientConnectionsMaxDeprecated);
}

static nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncMetricDbClientConnectionsMaxDeprecated(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableUpDownCounter(kMetricDbClientConnectionsMaxDeprecated,
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
static constexpr const char *descrMetricDbClientConnectionsPendingRequestsDeprecated =
    "Deprecated, use `db.client.connection.pending_requests` instead.";
static constexpr const char *unitMetricDbClientConnectionsPendingRequestsDeprecated = "{request}";

static nostd::unique_ptr<metrics::UpDownCounter<uint64_t>>
CreateSyncMetricDbClientConnectionsPendingRequestsDeprecated(metrics::Meter *meter)
{
  return meter->CreateInt64UpDownCounter(kMetricDbClientConnectionsPendingRequestsDeprecated,
                                         descrMetricDbClientConnectionsPendingRequestsDeprecated,
                                         unitMetricDbClientConnectionsPendingRequestsDeprecated);
}

static nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncMetricDbClientConnectionsPendingRequestsDeprecated(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableUpDownCounter(
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
static constexpr const char *descrMetricDbClientConnectionsTimeoutsDeprecated =
    "Deprecated, use `db.client.connection.timeouts` instead.";
static constexpr const char *unitMetricDbClientConnectionsTimeoutsDeprecated = "{timeout}";

static nostd::unique_ptr<metrics::Counter<uint64_t>>
CreateSyncMetricDbClientConnectionsTimeoutsDeprecated(metrics::Meter *meter)
{
  return meter->CreateUInt64Counter(kMetricDbClientConnectionsTimeoutsDeprecated,
                                    descrMetricDbClientConnectionsTimeoutsDeprecated,
                                    unitMetricDbClientConnectionsTimeoutsDeprecated);
}

static nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncMetricDbClientConnectionsTimeoutsDeprecated(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableCounter(kMetricDbClientConnectionsTimeoutsDeprecated,
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
static constexpr const char *descrMetricDbClientConnectionsCountDeprecated =
    "Deprecated, use `db.client.connection.count` instead.";
static constexpr const char *unitMetricDbClientConnectionsCountDeprecated = "{connection}";

static nostd::unique_ptr<metrics::UpDownCounter<uint64_t>>
CreateSyncMetricDbClientConnectionsCountDeprecated(metrics::Meter *meter)
{
  return meter->CreateInt64UpDownCounter(kMetricDbClientConnectionsCountDeprecated,
                                         descrMetricDbClientConnectionsCountDeprecated,
                                         unitMetricDbClientConnectionsCountDeprecated);
}

static nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncMetricDbClientConnectionsCountDeprecated(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableUpDownCounter(kMetricDbClientConnectionsCountDeprecated,
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
static constexpr const char *descrMetricDbClientConnectionsUseTimeDeprecated =
    "Deprecated, use `db.client.connection.use_time` instead. Note: the unit also changed from "
    "`ms` to `s`.";
static constexpr const char *unitMetricDbClientConnectionsUseTimeDeprecated = "ms";

static nostd::unique_ptr<metrics::Histogram<uint64_t>>
CreateSyncMetricDbClientConnectionsUseTimeDeprecated(metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricDbClientConnectionsUseTimeDeprecated,
                                      descrMetricDbClientConnectionsUseTimeDeprecated,
                                      unitMetricDbClientConnectionsUseTimeDeprecated);
}

static nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncMetricDbClientConnectionsUseTimeDeprecated(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableHistogram(kMetricDbClientConnectionsUseTimeDeprecated,
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
static constexpr const char *descrMetricDbClientConnectionsWaitTimeDeprecated =
    "Deprecated, use `db.client.connection.wait_time` instead. Note: the unit also changed from "
    "`ms` to `s`.";
static constexpr const char *unitMetricDbClientConnectionsWaitTimeDeprecated = "ms";

static nostd::unique_ptr<metrics::Histogram<uint64_t>>
CreateSyncMetricDbClientConnectionsWaitTimeDeprecated(metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricDbClientConnectionsWaitTimeDeprecated,
                                      descrMetricDbClientConnectionsWaitTimeDeprecated,
                                      unitMetricDbClientConnectionsWaitTimeDeprecated);
}

static nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncMetricDbClientConnectionsWaitTimeDeprecated(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableHistogram(kMetricDbClientConnectionsWaitTimeDeprecated,
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

static nostd::unique_ptr<metrics::Histogram<uint64_t>> CreateSyncMetricDbClientOperationDuration(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricDbClientOperationDuration,
                                      descrMetricDbClientOperationDuration,
                                      unitMetricDbClientOperationDuration);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricDbClientOperationDuration(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableHistogram(kMetricDbClientOperationDuration,
                                               descrMetricDbClientOperationDuration,
                                               unitMetricDbClientOperationDuration);
}

}  // namespace db
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
