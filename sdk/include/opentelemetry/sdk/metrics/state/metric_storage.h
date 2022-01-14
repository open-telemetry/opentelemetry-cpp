// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
# include "opentelemetry/sdk/metrics/"
#  include "opentelemetry/version.h"
# include "opentelemetry/sdk/metrics/state/sync_metrics_storage.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

/** Represents a storage from which we can collect metrics.
 *
 */
class MetricStorage{
    /*
     *   Collects the metrics from this storage.
     */
    virtual MetricData& collect(
        CollectionInfo &collection_info,
        std::vector<CollectorInfo> &collection_infos,
        opentelemetry::sdk::resource::Resource &resource,
        opentelemetry::sdk::instrumentationlibrary::InstrumentationLibraryInfo &instrumentationLibraryInfo,
        opentelemetry::common::SystemTimestamp& start_epoch_nanos,
        opentelemetry::common::SystemTimestamp& epoch_nanos
    ) noexcept = 0;
}
}
}
OPENTELEMETRY_END_NAMESPACE
#endif