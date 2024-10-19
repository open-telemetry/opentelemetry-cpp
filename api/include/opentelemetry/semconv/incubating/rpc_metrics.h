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

static nostd::unique_ptr<metrics::Histogram<uint64_t>> CreateSyncMetricRpcClientDuration(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricRpcClientDuration, descrMetricRpcClientDuration,
                                      unitMetricRpcClientDuration);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricRpcClientDuration(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableHistogram(
      kMetricRpcClientDuration, descrMetricRpcClientDuration, unitMetricRpcClientDuration);
}

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

static nostd::unique_ptr<metrics::Histogram<uint64_t>> CreateSyncMetricRpcClientRequestSize(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricRpcClientRequestSize, descrMetricRpcClientRequestSize,
                                      unitMetricRpcClientRequestSize);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricRpcClientRequestSize(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableHistogram(
      kMetricRpcClientRequestSize, descrMetricRpcClientRequestSize, unitMetricRpcClientRequestSize);
}

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

static nostd::unique_ptr<metrics::Histogram<uint64_t>> CreateSyncMetricRpcClientRequestsPerRpc(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricRpcClientRequestsPerRpc,
                                      descrMetricRpcClientRequestsPerRpc,
                                      unitMetricRpcClientRequestsPerRpc);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricRpcClientRequestsPerRpc(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableHistogram(kMetricRpcClientRequestsPerRpc,
                                               descrMetricRpcClientRequestsPerRpc,
                                               unitMetricRpcClientRequestsPerRpc);
}

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

static nostd::unique_ptr<metrics::Histogram<uint64_t>> CreateSyncMetricRpcClientResponseSize(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricRpcClientResponseSize,
                                      descrMetricRpcClientResponseSize,
                                      unitMetricRpcClientResponseSize);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricRpcClientResponseSize(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableHistogram(kMetricRpcClientResponseSize,
                                               descrMetricRpcClientResponseSize,
                                               unitMetricRpcClientResponseSize);
}

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

static nostd::unique_ptr<metrics::Histogram<uint64_t>> CreateSyncMetricRpcClientResponsesPerRpc(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricRpcClientResponsesPerRpc,
                                      descrMetricRpcClientResponsesPerRpc,
                                      unitMetricRpcClientResponsesPerRpc);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricRpcClientResponsesPerRpc(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableHistogram(kMetricRpcClientResponsesPerRpc,
                                               descrMetricRpcClientResponsesPerRpc,
                                               unitMetricRpcClientResponsesPerRpc);
}

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

static nostd::unique_ptr<metrics::Histogram<uint64_t>> CreateSyncMetricRpcServerDuration(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricRpcServerDuration, descrMetricRpcServerDuration,
                                      unitMetricRpcServerDuration);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricRpcServerDuration(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableHistogram(
      kMetricRpcServerDuration, descrMetricRpcServerDuration, unitMetricRpcServerDuration);
}

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

static nostd::unique_ptr<metrics::Histogram<uint64_t>> CreateSyncMetricRpcServerRequestSize(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricRpcServerRequestSize, descrMetricRpcServerRequestSize,
                                      unitMetricRpcServerRequestSize);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricRpcServerRequestSize(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableHistogram(
      kMetricRpcServerRequestSize, descrMetricRpcServerRequestSize, unitMetricRpcServerRequestSize);
}

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

static nostd::unique_ptr<metrics::Histogram<uint64_t>> CreateSyncMetricRpcServerRequestsPerRpc(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricRpcServerRequestsPerRpc,
                                      descrMetricRpcServerRequestsPerRpc,
                                      unitMetricRpcServerRequestsPerRpc);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricRpcServerRequestsPerRpc(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableHistogram(kMetricRpcServerRequestsPerRpc,
                                               descrMetricRpcServerRequestsPerRpc,
                                               unitMetricRpcServerRequestsPerRpc);
}

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

static nostd::unique_ptr<metrics::Histogram<uint64_t>> CreateSyncMetricRpcServerResponseSize(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricRpcServerResponseSize,
                                      descrMetricRpcServerResponseSize,
                                      unitMetricRpcServerResponseSize);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricRpcServerResponseSize(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableHistogram(kMetricRpcServerResponseSize,
                                               descrMetricRpcServerResponseSize,
                                               unitMetricRpcServerResponseSize);
}

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

static nostd::unique_ptr<metrics::Histogram<uint64_t>> CreateSyncMetricRpcServerResponsesPerRpc(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricRpcServerResponsesPerRpc,
                                      descrMetricRpcServerResponsesPerRpc,
                                      unitMetricRpcServerResponsesPerRpc);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricRpcServerResponsesPerRpc(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableHistogram(kMetricRpcServerResponsesPerRpc,
                                               descrMetricRpcServerResponsesPerRpc,
                                               unitMetricRpcServerResponsesPerRpc);
}

}  // namespace rpc
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
