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
namespace gen_ai
{

/**
 * GenAI operation duration
 * <p>
 * histogram
 */
static constexpr const char *kMetricGenAiClientOperationDuration =
    "metric.gen_ai.client.operation.duration";
static constexpr const char *descrMetricGenAiClientOperationDuration = "GenAI operation duration";
static constexpr const char *unitMetricGenAiClientOperationDuration  = "s";

static nostd::unique_ptr<metrics::Histogram<uint64_t>> CreateSyncMetricGenAiClientOperationDuration(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricGenAiClientOperationDuration,
                                      descrMetricGenAiClientOperationDuration,
                                      unitMetricGenAiClientOperationDuration);
}

static nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncMetricGenAiClientOperationDuration(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableHistogram(kMetricGenAiClientOperationDuration,
                                               descrMetricGenAiClientOperationDuration,
                                               unitMetricGenAiClientOperationDuration);
}

/**
 * Measures number of input and output tokens used
 * <p>
 * histogram
 */
static constexpr const char *kMetricGenAiClientTokenUsage = "metric.gen_ai.client.token.usage";
static constexpr const char *descrMetricGenAiClientTokenUsage =
    "Measures number of input and output tokens used";
static constexpr const char *unitMetricGenAiClientTokenUsage = "{token}";

static nostd::unique_ptr<metrics::Histogram<uint64_t>> CreateSyncMetricGenAiClientTokenUsage(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricGenAiClientTokenUsage,
                                      descrMetricGenAiClientTokenUsage,
                                      unitMetricGenAiClientTokenUsage);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricGenAiClientTokenUsage(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableHistogram(kMetricGenAiClientTokenUsage,
                                               descrMetricGenAiClientTokenUsage,
                                               unitMetricGenAiClientTokenUsage);
}

/**
 * Generative AI server request duration such as time-to-last byte or last output token
 * <p>
 * histogram
 */
static constexpr const char *kMetricGenAiServerRequestDuration =
    "metric.gen_ai.server.request.duration";
static constexpr const char *descrMetricGenAiServerRequestDuration =
    "Generative AI server request duration such as time-to-last byte or last output token";
static constexpr const char *unitMetricGenAiServerRequestDuration = "s";

static nostd::unique_ptr<metrics::Histogram<uint64_t>> CreateSyncMetricGenAiServerRequestDuration(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricGenAiServerRequestDuration,
                                      descrMetricGenAiServerRequestDuration,
                                      unitMetricGenAiServerRequestDuration);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricGenAiServerRequestDuration(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableHistogram(kMetricGenAiServerRequestDuration,
                                               descrMetricGenAiServerRequestDuration,
                                               unitMetricGenAiServerRequestDuration);
}

/**
 * Time per output token generated after the first token for successful responses
 * <p>
 * histogram
 */
static constexpr const char *kMetricGenAiServerTimePerOutputToken =
    "metric.gen_ai.server.time_per_output_token";
static constexpr const char *descrMetricGenAiServerTimePerOutputToken =
    "Time per output token generated after the first token for successful responses";
static constexpr const char *unitMetricGenAiServerTimePerOutputToken = "s";

static nostd::unique_ptr<metrics::Histogram<uint64_t>>
CreateSyncMetricGenAiServerTimePerOutputToken(metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricGenAiServerTimePerOutputToken,
                                      descrMetricGenAiServerTimePerOutputToken,
                                      unitMetricGenAiServerTimePerOutputToken);
}

static nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncMetricGenAiServerTimePerOutputToken(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableHistogram(kMetricGenAiServerTimePerOutputToken,
                                               descrMetricGenAiServerTimePerOutputToken,
                                               unitMetricGenAiServerTimePerOutputToken);
}

/**
 * Time to generate first token for successful responses
 * <p>
 * histogram
 */
static constexpr const char *kMetricGenAiServerTimeToFirstToken =
    "metric.gen_ai.server.time_to_first_token";
static constexpr const char *descrMetricGenAiServerTimeToFirstToken =
    "Time to generate first token for successful responses";
static constexpr const char *unitMetricGenAiServerTimeToFirstToken = "s";

static nostd::unique_ptr<metrics::Histogram<uint64_t>> CreateSyncMetricGenAiServerTimeToFirstToken(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricGenAiServerTimeToFirstToken,
                                      descrMetricGenAiServerTimeToFirstToken,
                                      unitMetricGenAiServerTimeToFirstToken);
}

static nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncMetricGenAiServerTimeToFirstToken(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableHistogram(kMetricGenAiServerTimeToFirstToken,
                                               descrMetricGenAiServerTimeToFirstToken,
                                               unitMetricGenAiServerTimeToFirstToken);
}

}  // namespace gen_ai
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
