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
  Measures the duration of an outgoing Remote Procedure Call (RPC).
  <p>
  When this metric is reported alongside an RPC client span, the metric value
  SHOULD be the same as the RPC client span duration.
  <p>
  histogram
 */
static constexpr const char *kMetricRpcClientCallDuration = "rpc.client.call.duration";
static constexpr const char *descrMetricRpcClientCallDuration =
    "Measures the duration of an outgoing Remote Procedure Call (RPC).";
static constexpr const char *unitMetricRpcClientCallDuration = "s";

static inline nostd::unique_ptr<metrics::Histogram<uint64_t>>
CreateSyncInt64MetricRpcClientCallDuration(metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricRpcClientCallDuration,
                                      descrMetricRpcClientCallDuration,
                                      unitMetricRpcClientCallDuration);
}

static inline nostd::unique_ptr<metrics::Histogram<double>>
CreateSyncDoubleMetricRpcClientCallDuration(metrics::Meter *meter)
{
  return meter->CreateDoubleHistogram(kMetricRpcClientCallDuration,
                                      descrMetricRpcClientCallDuration,
                                      unitMetricRpcClientCallDuration);
}

/**
  Measures the duration of an incoming Remote Procedure Call (RPC).
  <p>
  When this metric is reported alongside an RPC server span, the metric value
  SHOULD be the same as the RPC server span duration.
  <p>
  histogram
 */
static constexpr const char *kMetricRpcServerCallDuration = "rpc.server.call.duration";
static constexpr const char *descrMetricRpcServerCallDuration =
    "Measures the duration of an incoming Remote Procedure Call (RPC).";
static constexpr const char *unitMetricRpcServerCallDuration = "s";

static inline nostd::unique_ptr<metrics::Histogram<uint64_t>>
CreateSyncInt64MetricRpcServerCallDuration(metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricRpcServerCallDuration,
                                      descrMetricRpcServerCallDuration,
                                      unitMetricRpcServerCallDuration);
}

static inline nostd::unique_ptr<metrics::Histogram<double>>
CreateSyncDoubleMetricRpcServerCallDuration(metrics::Meter *meter)
{
  return meter->CreateDoubleHistogram(kMetricRpcServerCallDuration,
                                      descrMetricRpcServerCallDuration,
                                      unitMetricRpcServerCallDuration);
}

}  // namespace rpc
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
