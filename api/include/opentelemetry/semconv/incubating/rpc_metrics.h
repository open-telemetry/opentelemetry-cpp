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
namespace rpc
{

/**
 * Measures the duration of outbound RPC.
 * <p>
 * While streaming RPCs may record this metric as start-of-batch
 * to end-of-batch, it's hard to interpret in practice.
 * <p>
 * <strong>Streaming</strong>: N/A.
 * <p>
 * histogram
 */
static constexpr const char *kMetricRpcClientDuration = "metric.rpc.client.duration";
static constexpr const char *descrMetricRpcClientDuration =
    "Measures the duration of outbound RPC.";
static constexpr const char *unitMetricRpcClientDuration = "ms";

static inline nostd::unique_ptr<metrics::Histogram<uint64_t>>
CreateSyncInt64MetricRpcClientDuration(metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricRpcClientDuration, descrMetricRpcClientDuration,
                                      unitMetricRpcClientDuration);
}

static inline nostd::unique_ptr<metrics::Histogram<double>> CreateSyncDoubleMetricRpcClientDuration(
    metrics::Meter *meter)
{
  return meter->CreateDoubleHistogram(kMetricRpcClientDuration, descrMetricRpcClientDuration,
                                      unitMetricRpcClientDuration);
}

#ifdef OPENTELEMETRY_LATER
// Unsupported: Async histogram
static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncInt64MetricRpcClientDuration(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableHistogram(
      kMetricRpcClientDuration, descrMetricRpcClientDuration, unitMetricRpcClientDuration);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncDoubleMetricRpcClientDuration(metrics::Meter *meter)
{
  return meter->CreateDoubleObservableHistogram(
      kMetricRpcClientDuration, descrMetricRpcClientDuration, unitMetricRpcClientDuration);
}
#endif /* OPENTELEMETRY_LATER */

/**
 * Measures the size of RPC request messages (uncompressed).
 * <p>
 * <strong>Streaming</strong>: Recorded per message in a streaming batch
 * <p>
 * histogram
 */
static constexpr const char *kMetricRpcClientRequestSize = "metric.rpc.client.request.size";
static constexpr const char *descrMetricRpcClientRequestSize =
    "Measures the size of RPC request messages (uncompressed).";
static constexpr const char *unitMetricRpcClientRequestSize = "By";

static inline nostd::unique_ptr<metrics::Histogram<uint64_t>>
CreateSyncInt64MetricRpcClientRequestSize(metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricRpcClientRequestSize, descrMetricRpcClientRequestSize,
                                      unitMetricRpcClientRequestSize);
}

static inline nostd::unique_ptr<metrics::Histogram<double>>
CreateSyncDoubleMetricRpcClientRequestSize(metrics::Meter *meter)
{
  return meter->CreateDoubleHistogram(kMetricRpcClientRequestSize, descrMetricRpcClientRequestSize,
                                      unitMetricRpcClientRequestSize);
}

#ifdef OPENTELEMETRY_LATER
// Unsupported: Async histogram
static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncInt64MetricRpcClientRequestSize(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableHistogram(
      kMetricRpcClientRequestSize, descrMetricRpcClientRequestSize, unitMetricRpcClientRequestSize);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncDoubleMetricRpcClientRequestSize(metrics::Meter *meter)
{
  return meter->CreateDoubleObservableHistogram(
      kMetricRpcClientRequestSize, descrMetricRpcClientRequestSize, unitMetricRpcClientRequestSize);
}
#endif /* OPENTELEMETRY_LATER */

/**
 * Measures the number of messages received per RPC.
 * <p>
 * Should be 1 for all non-streaming RPCs.
 * <p>
 * <strong>Streaming</strong>: This metric is required for server and client streaming RPCs
 * <p>
 * histogram
 */
static constexpr const char *kMetricRpcClientRequestsPerRpc = "metric.rpc.client.requests_per_rpc";
static constexpr const char *descrMetricRpcClientRequestsPerRpc =
    "Measures the number of messages received per RPC.";
static constexpr const char *unitMetricRpcClientRequestsPerRpc = "{count}";

static inline nostd::unique_ptr<metrics::Histogram<uint64_t>>
CreateSyncInt64MetricRpcClientRequestsPerRpc(metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricRpcClientRequestsPerRpc,
                                      descrMetricRpcClientRequestsPerRpc,
                                      unitMetricRpcClientRequestsPerRpc);
}

static inline nostd::unique_ptr<metrics::Histogram<double>>
CreateSyncDoubleMetricRpcClientRequestsPerRpc(metrics::Meter *meter)
{
  return meter->CreateDoubleHistogram(kMetricRpcClientRequestsPerRpc,
                                      descrMetricRpcClientRequestsPerRpc,
                                      unitMetricRpcClientRequestsPerRpc);
}

#ifdef OPENTELEMETRY_LATER
// Unsupported: Async histogram
static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncInt64MetricRpcClientRequestsPerRpc(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableHistogram(kMetricRpcClientRequestsPerRpc,
                                               descrMetricRpcClientRequestsPerRpc,
                                               unitMetricRpcClientRequestsPerRpc);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncDoubleMetricRpcClientRequestsPerRpc(metrics::Meter *meter)
{
  return meter->CreateDoubleObservableHistogram(kMetricRpcClientRequestsPerRpc,
                                                descrMetricRpcClientRequestsPerRpc,
                                                unitMetricRpcClientRequestsPerRpc);
}
#endif /* OPENTELEMETRY_LATER */

/**
 * Measures the size of RPC response messages (uncompressed).
 * <p>
 * <strong>Streaming</strong>: Recorded per response in a streaming batch
 * <p>
 * histogram
 */
static constexpr const char *kMetricRpcClientResponseSize = "metric.rpc.client.response.size";
static constexpr const char *descrMetricRpcClientResponseSize =
    "Measures the size of RPC response messages (uncompressed).";
static constexpr const char *unitMetricRpcClientResponseSize = "By";

static inline nostd::unique_ptr<metrics::Histogram<uint64_t>>
CreateSyncInt64MetricRpcClientResponseSize(metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricRpcClientResponseSize,
                                      descrMetricRpcClientResponseSize,
                                      unitMetricRpcClientResponseSize);
}

static inline nostd::unique_ptr<metrics::Histogram<double>>
CreateSyncDoubleMetricRpcClientResponseSize(metrics::Meter *meter)
{
  return meter->CreateDoubleHistogram(kMetricRpcClientResponseSize,
                                      descrMetricRpcClientResponseSize,
                                      unitMetricRpcClientResponseSize);
}

#ifdef OPENTELEMETRY_LATER
// Unsupported: Async histogram
static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncInt64MetricRpcClientResponseSize(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableHistogram(kMetricRpcClientResponseSize,
                                               descrMetricRpcClientResponseSize,
                                               unitMetricRpcClientResponseSize);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncDoubleMetricRpcClientResponseSize(metrics::Meter *meter)
{
  return meter->CreateDoubleObservableHistogram(kMetricRpcClientResponseSize,
                                                descrMetricRpcClientResponseSize,
                                                unitMetricRpcClientResponseSize);
}
#endif /* OPENTELEMETRY_LATER */

/**
 * Measures the number of messages sent per RPC.
 * <p>
 * Should be 1 for all non-streaming RPCs.
 * <p>
 * <strong>Streaming</strong>: This metric is required for server and client streaming RPCs
 * <p>
 * histogram
 */
static constexpr const char *kMetricRpcClientResponsesPerRpc =
    "metric.rpc.client.responses_per_rpc";
static constexpr const char *descrMetricRpcClientResponsesPerRpc =
    "Measures the number of messages sent per RPC.";
static constexpr const char *unitMetricRpcClientResponsesPerRpc = "{count}";

static inline nostd::unique_ptr<metrics::Histogram<uint64_t>>
CreateSyncInt64MetricRpcClientResponsesPerRpc(metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricRpcClientResponsesPerRpc,
                                      descrMetricRpcClientResponsesPerRpc,
                                      unitMetricRpcClientResponsesPerRpc);
}

static inline nostd::unique_ptr<metrics::Histogram<double>>
CreateSyncDoubleMetricRpcClientResponsesPerRpc(metrics::Meter *meter)
{
  return meter->CreateDoubleHistogram(kMetricRpcClientResponsesPerRpc,
                                      descrMetricRpcClientResponsesPerRpc,
                                      unitMetricRpcClientResponsesPerRpc);
}

#ifdef OPENTELEMETRY_LATER
// Unsupported: Async histogram
static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncInt64MetricRpcClientResponsesPerRpc(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableHistogram(kMetricRpcClientResponsesPerRpc,
                                               descrMetricRpcClientResponsesPerRpc,
                                               unitMetricRpcClientResponsesPerRpc);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncDoubleMetricRpcClientResponsesPerRpc(metrics::Meter *meter)
{
  return meter->CreateDoubleObservableHistogram(kMetricRpcClientResponsesPerRpc,
                                                descrMetricRpcClientResponsesPerRpc,
                                                unitMetricRpcClientResponsesPerRpc);
}
#endif /* OPENTELEMETRY_LATER */

/**
 * Measures the duration of inbound RPC.
 * <p>
 * While streaming RPCs may record this metric as start-of-batch
 * to end-of-batch, it's hard to interpret in practice.
 * <p>
 * <strong>Streaming</strong>: N/A.
 * <p>
 * histogram
 */
static constexpr const char *kMetricRpcServerDuration     = "metric.rpc.server.duration";
static constexpr const char *descrMetricRpcServerDuration = "Measures the duration of inbound RPC.";
static constexpr const char *unitMetricRpcServerDuration  = "ms";

static inline nostd::unique_ptr<metrics::Histogram<uint64_t>>
CreateSyncInt64MetricRpcServerDuration(metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricRpcServerDuration, descrMetricRpcServerDuration,
                                      unitMetricRpcServerDuration);
}

static inline nostd::unique_ptr<metrics::Histogram<double>> CreateSyncDoubleMetricRpcServerDuration(
    metrics::Meter *meter)
{
  return meter->CreateDoubleHistogram(kMetricRpcServerDuration, descrMetricRpcServerDuration,
                                      unitMetricRpcServerDuration);
}

#ifdef OPENTELEMETRY_LATER
// Unsupported: Async histogram
static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncInt64MetricRpcServerDuration(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableHistogram(
      kMetricRpcServerDuration, descrMetricRpcServerDuration, unitMetricRpcServerDuration);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncDoubleMetricRpcServerDuration(metrics::Meter *meter)
{
  return meter->CreateDoubleObservableHistogram(
      kMetricRpcServerDuration, descrMetricRpcServerDuration, unitMetricRpcServerDuration);
}
#endif /* OPENTELEMETRY_LATER */

/**
 * Measures the size of RPC request messages (uncompressed).
 * <p>
 * <strong>Streaming</strong>: Recorded per message in a streaming batch
 * <p>
 * histogram
 */
static constexpr const char *kMetricRpcServerRequestSize = "metric.rpc.server.request.size";
static constexpr const char *descrMetricRpcServerRequestSize =
    "Measures the size of RPC request messages (uncompressed).";
static constexpr const char *unitMetricRpcServerRequestSize = "By";

static inline nostd::unique_ptr<metrics::Histogram<uint64_t>>
CreateSyncInt64MetricRpcServerRequestSize(metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricRpcServerRequestSize, descrMetricRpcServerRequestSize,
                                      unitMetricRpcServerRequestSize);
}

static inline nostd::unique_ptr<metrics::Histogram<double>>
CreateSyncDoubleMetricRpcServerRequestSize(metrics::Meter *meter)
{
  return meter->CreateDoubleHistogram(kMetricRpcServerRequestSize, descrMetricRpcServerRequestSize,
                                      unitMetricRpcServerRequestSize);
}

#ifdef OPENTELEMETRY_LATER
// Unsupported: Async histogram
static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncInt64MetricRpcServerRequestSize(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableHistogram(
      kMetricRpcServerRequestSize, descrMetricRpcServerRequestSize, unitMetricRpcServerRequestSize);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncDoubleMetricRpcServerRequestSize(metrics::Meter *meter)
{
  return meter->CreateDoubleObservableHistogram(
      kMetricRpcServerRequestSize, descrMetricRpcServerRequestSize, unitMetricRpcServerRequestSize);
}
#endif /* OPENTELEMETRY_LATER */

/**
 * Measures the number of messages received per RPC.
 * <p>
 * Should be 1 for all non-streaming RPCs.
 * <p>
 * <strong>Streaming</strong> : This metric is required for server and client streaming RPCs
 * <p>
 * histogram
 */
static constexpr const char *kMetricRpcServerRequestsPerRpc = "metric.rpc.server.requests_per_rpc";
static constexpr const char *descrMetricRpcServerRequestsPerRpc =
    "Measures the number of messages received per RPC.";
static constexpr const char *unitMetricRpcServerRequestsPerRpc = "{count}";

static inline nostd::unique_ptr<metrics::Histogram<uint64_t>>
CreateSyncInt64MetricRpcServerRequestsPerRpc(metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricRpcServerRequestsPerRpc,
                                      descrMetricRpcServerRequestsPerRpc,
                                      unitMetricRpcServerRequestsPerRpc);
}

static inline nostd::unique_ptr<metrics::Histogram<double>>
CreateSyncDoubleMetricRpcServerRequestsPerRpc(metrics::Meter *meter)
{
  return meter->CreateDoubleHistogram(kMetricRpcServerRequestsPerRpc,
                                      descrMetricRpcServerRequestsPerRpc,
                                      unitMetricRpcServerRequestsPerRpc);
}

#ifdef OPENTELEMETRY_LATER
// Unsupported: Async histogram
static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncInt64MetricRpcServerRequestsPerRpc(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableHistogram(kMetricRpcServerRequestsPerRpc,
                                               descrMetricRpcServerRequestsPerRpc,
                                               unitMetricRpcServerRequestsPerRpc);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncDoubleMetricRpcServerRequestsPerRpc(metrics::Meter *meter)
{
  return meter->CreateDoubleObservableHistogram(kMetricRpcServerRequestsPerRpc,
                                                descrMetricRpcServerRequestsPerRpc,
                                                unitMetricRpcServerRequestsPerRpc);
}
#endif /* OPENTELEMETRY_LATER */

/**
 * Measures the size of RPC response messages (uncompressed).
 * <p>
 * <strong>Streaming</strong>: Recorded per response in a streaming batch
 * <p>
 * histogram
 */
static constexpr const char *kMetricRpcServerResponseSize = "metric.rpc.server.response.size";
static constexpr const char *descrMetricRpcServerResponseSize =
    "Measures the size of RPC response messages (uncompressed).";
static constexpr const char *unitMetricRpcServerResponseSize = "By";

static inline nostd::unique_ptr<metrics::Histogram<uint64_t>>
CreateSyncInt64MetricRpcServerResponseSize(metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricRpcServerResponseSize,
                                      descrMetricRpcServerResponseSize,
                                      unitMetricRpcServerResponseSize);
}

static inline nostd::unique_ptr<metrics::Histogram<double>>
CreateSyncDoubleMetricRpcServerResponseSize(metrics::Meter *meter)
{
  return meter->CreateDoubleHistogram(kMetricRpcServerResponseSize,
                                      descrMetricRpcServerResponseSize,
                                      unitMetricRpcServerResponseSize);
}

#ifdef OPENTELEMETRY_LATER
// Unsupported: Async histogram
static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncInt64MetricRpcServerResponseSize(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableHistogram(kMetricRpcServerResponseSize,
                                               descrMetricRpcServerResponseSize,
                                               unitMetricRpcServerResponseSize);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncDoubleMetricRpcServerResponseSize(metrics::Meter *meter)
{
  return meter->CreateDoubleObservableHistogram(kMetricRpcServerResponseSize,
                                                descrMetricRpcServerResponseSize,
                                                unitMetricRpcServerResponseSize);
}
#endif /* OPENTELEMETRY_LATER */

/**
 * Measures the number of messages sent per RPC.
 * <p>
 * Should be 1 for all non-streaming RPCs.
 * <p>
 * <strong>Streaming</strong>: This metric is required for server and client streaming RPCs
 * <p>
 * histogram
 */
static constexpr const char *kMetricRpcServerResponsesPerRpc =
    "metric.rpc.server.responses_per_rpc";
static constexpr const char *descrMetricRpcServerResponsesPerRpc =
    "Measures the number of messages sent per RPC.";
static constexpr const char *unitMetricRpcServerResponsesPerRpc = "{count}";

static inline nostd::unique_ptr<metrics::Histogram<uint64_t>>
CreateSyncInt64MetricRpcServerResponsesPerRpc(metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricRpcServerResponsesPerRpc,
                                      descrMetricRpcServerResponsesPerRpc,
                                      unitMetricRpcServerResponsesPerRpc);
}

static inline nostd::unique_ptr<metrics::Histogram<double>>
CreateSyncDoubleMetricRpcServerResponsesPerRpc(metrics::Meter *meter)
{
  return meter->CreateDoubleHistogram(kMetricRpcServerResponsesPerRpc,
                                      descrMetricRpcServerResponsesPerRpc,
                                      unitMetricRpcServerResponsesPerRpc);
}

#ifdef OPENTELEMETRY_LATER
// Unsupported: Async histogram
static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncInt64MetricRpcServerResponsesPerRpc(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableHistogram(kMetricRpcServerResponsesPerRpc,
                                               descrMetricRpcServerResponsesPerRpc,
                                               unitMetricRpcServerResponsesPerRpc);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncDoubleMetricRpcServerResponsesPerRpc(metrics::Meter *meter)
{
  return meter->CreateDoubleObservableHistogram(kMetricRpcServerResponsesPerRpc,
                                                descrMetricRpcServerResponsesPerRpc,
                                                unitMetricRpcServerResponsesPerRpc);
}
#endif /* OPENTELEMETRY_LATER */

}  // namespace rpc
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
