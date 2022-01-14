// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/version.h"
# include "opentelemetry/sdk/metrics/state/sync_metrics_storage.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
 /*
 * Stores and aggregates {@link MetricData} for synchronous instruments.
 */

template <class T>
class SyncMetricStorage : public WritableMetricStorage<T>, MetricsStorage
{

    void Record(T value) noexcept override {

    }

    void Record(T value, const common::KeyValueIterable &attributes) noexcept override {

    }

    /* Collects the metrics from this storage.*/
    MetricData& Collect(
        CollectionInfo &collection_info,
        std::vector<CollectorInfo> &collection_infos,
        opentelemetry::sdk::resource::Resource &resource,
        opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary &instrumentation_library,
        opentelemetry::common::SystemTimestamp& start_epoch_nanos,
        opentelemetry::common::SystemTimestamp& epoch_nanos       
    )
    {

    }
}
}
}
OPENTELEMETRY_END_NAMESPACE
#endif
