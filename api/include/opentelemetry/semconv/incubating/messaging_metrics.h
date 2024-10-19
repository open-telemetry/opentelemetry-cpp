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
namespace messaging
{

/**
 * Number of messages that were delivered to the application.
 * <p>
 * Records the number of messages pulled from the broker or number of messages dispatched to the
 * application in push-based scenarios. The metric SHOULD be reported once per message delivery. For
 * example, if receiving and processing operations are both instrumented for a single message
 * delivery, this counter is incremented when the message is received and not reported when it is
 * processed. <p> counter
 */
static constexpr const char *kMetricMessagingClientConsumedMessages =
    "metric.messaging.client.consumed.messages";
static constexpr const char *descrMetricMessagingClientConsumedMessages =
    "Number of messages that were delivered to the application.";
static constexpr const char *unitMetricMessagingClientConsumedMessages = "{message}";

static nostd::unique_ptr<metrics::Counter<uint64_t>>
CreateSyncMetricMessagingClientConsumedMessages(metrics::Meter *meter)
{
  return meter->CreateUInt64Counter(kMetricMessagingClientConsumedMessages,
                                    descrMetricMessagingClientConsumedMessages,
                                    unitMetricMessagingClientConsumedMessages);
}

static nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncMetricMessagingClientConsumedMessages(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableCounter(kMetricMessagingClientConsumedMessages,
                                             descrMetricMessagingClientConsumedMessages,
                                             unitMetricMessagingClientConsumedMessages);
}

/**
 * Duration of messaging operation initiated by a producer or consumer client.
 * <p>
 * This metric SHOULD NOT be used to report processing duration - processing duration is reported in
 * @code messaging.process.duration @endcode metric. <p> histogram
 */
static constexpr const char *kMetricMessagingClientOperationDuration =
    "metric.messaging.client.operation.duration";
static constexpr const char *descrMetricMessagingClientOperationDuration =
    "Duration of messaging operation initiated by a producer or consumer client.";
static constexpr const char *unitMetricMessagingClientOperationDuration = "s";

static nostd::unique_ptr<metrics::Histogram<uint64_t>>
CreateSyncMetricMessagingClientOperationDuration(metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricMessagingClientOperationDuration,
                                      descrMetricMessagingClientOperationDuration,
                                      unitMetricMessagingClientOperationDuration);
}

static nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncMetricMessagingClientOperationDuration(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableHistogram(kMetricMessagingClientOperationDuration,
                                               descrMetricMessagingClientOperationDuration,
                                               unitMetricMessagingClientOperationDuration);
}

/**
 * Number of messages producer attempted to publish to the broker.
 * <p>
 * This metric MUST NOT count messages that were created haven't yet been attempted to be published.
 * <p>
 * counter
 */
static constexpr const char *kMetricMessagingClientPublishedMessages =
    "metric.messaging.client.published.messages";
static constexpr const char *descrMetricMessagingClientPublishedMessages =
    "Number of messages producer attempted to publish to the broker.";
static constexpr const char *unitMetricMessagingClientPublishedMessages = "{message}";

static nostd::unique_ptr<metrics::Counter<uint64_t>>
CreateSyncMetricMessagingClientPublishedMessages(metrics::Meter *meter)
{
  return meter->CreateUInt64Counter(kMetricMessagingClientPublishedMessages,
                                    descrMetricMessagingClientPublishedMessages,
                                    unitMetricMessagingClientPublishedMessages);
}

static nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncMetricMessagingClientPublishedMessages(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableCounter(kMetricMessagingClientPublishedMessages,
                                             descrMetricMessagingClientPublishedMessages,
                                             unitMetricMessagingClientPublishedMessages);
}

/**
 * Duration of processing operation.
 * <p>
 * This metric MUST be reported for operations with @code messaging.operation.type @endcode that
 * matches @code process @endcode. <p> histogram
 */
static constexpr const char *kMetricMessagingProcessDuration = "metric.messaging.process.duration";
static constexpr const char *descrMetricMessagingProcessDuration =
    "Duration of processing operation.";
static constexpr const char *unitMetricMessagingProcessDuration = "s";

static nostd::unique_ptr<metrics::Histogram<uint64_t>> CreateSyncMetricMessagingProcessDuration(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricMessagingProcessDuration,
                                      descrMetricMessagingProcessDuration,
                                      unitMetricMessagingProcessDuration);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricMessagingProcessDuration(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableHistogram(kMetricMessagingProcessDuration,
                                               descrMetricMessagingProcessDuration,
                                               unitMetricMessagingProcessDuration);
}

/**
 * Deprecated. Use @code messaging.client.consumed.messages @endcode instead.
 * <p>
 * @deprecated
 * Replaced by @code messaging.client.consumed.messages @endcode.
 * <p>
 * counter
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kMetricMessagingProcessMessages = "metric.messaging.process.messages";
static constexpr const char *descrMetricMessagingProcessMessages =
    "Deprecated. Use `messaging.client.consumed.messages` instead.";
static constexpr const char *unitMetricMessagingProcessMessages = "{message}";

static nostd::unique_ptr<metrics::Counter<uint64_t>> CreateSyncMetricMessagingProcessMessages(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Counter(kMetricMessagingProcessMessages,
                                    descrMetricMessagingProcessMessages,
                                    unitMetricMessagingProcessMessages);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricMessagingProcessMessages(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableCounter(kMetricMessagingProcessMessages,
                                             descrMetricMessagingProcessMessages,
                                             unitMetricMessagingProcessMessages);
}

/**
 * Deprecated. Use @code messaging.client.operation.duration @endcode instead.
 * <p>
 * @deprecated
 * Replaced by @code messaging.client.operation.duration @endcode.
 * <p>
 * histogram
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kMetricMessagingPublishDuration = "metric.messaging.publish.duration";
static constexpr const char *descrMetricMessagingPublishDuration =
    "Deprecated. Use `messaging.client.operation.duration` instead.";
static constexpr const char *unitMetricMessagingPublishDuration = "s";

static nostd::unique_ptr<metrics::Histogram<uint64_t>> CreateSyncMetricMessagingPublishDuration(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricMessagingPublishDuration,
                                      descrMetricMessagingPublishDuration,
                                      unitMetricMessagingPublishDuration);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricMessagingPublishDuration(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableHistogram(kMetricMessagingPublishDuration,
                                               descrMetricMessagingPublishDuration,
                                               unitMetricMessagingPublishDuration);
}

/**
 * Deprecated. Use @code messaging.client.produced.messages @endcode instead.
 * <p>
 * @deprecated
 * Replaced by @code messaging.client.produced.messages @endcode.
 * <p>
 * counter
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kMetricMessagingPublishMessages = "metric.messaging.publish.messages";
static constexpr const char *descrMetricMessagingPublishMessages =
    "Deprecated. Use `messaging.client.produced.messages` instead.";
static constexpr const char *unitMetricMessagingPublishMessages = "{message}";

static nostd::unique_ptr<metrics::Counter<uint64_t>> CreateSyncMetricMessagingPublishMessages(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Counter(kMetricMessagingPublishMessages,
                                    descrMetricMessagingPublishMessages,
                                    unitMetricMessagingPublishMessages);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricMessagingPublishMessages(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableCounter(kMetricMessagingPublishMessages,
                                             descrMetricMessagingPublishMessages,
                                             unitMetricMessagingPublishMessages);
}

/**
 * Deprecated. Use @code messaging.client.operation.duration @endcode instead.
 * <p>
 * @deprecated
 * Replaced by @code messaging.client.operation.duration @endcode.
 * <p>
 * histogram
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kMetricMessagingReceiveDuration = "metric.messaging.receive.duration";
static constexpr const char *descrMetricMessagingReceiveDuration =
    "Deprecated. Use `messaging.client.operation.duration` instead.";
static constexpr const char *unitMetricMessagingReceiveDuration = "s";

static nostd::unique_ptr<metrics::Histogram<uint64_t>> CreateSyncMetricMessagingReceiveDuration(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricMessagingReceiveDuration,
                                      descrMetricMessagingReceiveDuration,
                                      unitMetricMessagingReceiveDuration);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricMessagingReceiveDuration(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableHistogram(kMetricMessagingReceiveDuration,
                                               descrMetricMessagingReceiveDuration,
                                               unitMetricMessagingReceiveDuration);
}

/**
 * Deprecated. Use @code messaging.client.consumed.messages @endcode instead.
 * <p>
 * @deprecated
 * Replaced by @code messaging.client.consumed.messages @endcode.
 * <p>
 * counter
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kMetricMessagingReceiveMessages = "metric.messaging.receive.messages";
static constexpr const char *descrMetricMessagingReceiveMessages =
    "Deprecated. Use `messaging.client.consumed.messages` instead.";
static constexpr const char *unitMetricMessagingReceiveMessages = "{message}";

static nostd::unique_ptr<metrics::Counter<uint64_t>> CreateSyncMetricMessagingReceiveMessages(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Counter(kMetricMessagingReceiveMessages,
                                    descrMetricMessagingReceiveMessages,
                                    unitMetricMessagingReceiveMessages);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricMessagingReceiveMessages(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableCounter(kMetricMessagingReceiveMessages,
                                             descrMetricMessagingReceiveMessages,
                                             unitMetricMessagingReceiveMessages);
}

}  // namespace messaging
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
