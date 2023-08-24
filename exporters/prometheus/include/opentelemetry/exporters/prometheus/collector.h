// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>
#include <mutex>
#include <vector>

#include <prometheus/collectable.h>
#include <prometheus/metric_family.h>
#include "opentelemetry/exporters/prometheus/exporter_utils.h"
#include "opentelemetry/sdk/metrics/metric_reader.h"

namespace prometheus_client = ::prometheus;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace metrics
{
/**
 * The Prometheus Collector maintains the intermediate collection in Prometheus Exporter
 */
class PrometheusCollector : public prometheus_client::Collectable
{
public:
  /**
   * Default Constructor.
   *
   * This constructor initializes the collection for metrics to export
   * in this class with default capacity
   */
  explicit PrometheusCollector(sdk::metrics::MetricReader *reader);

  /**
   * Collects all metrics data from metricsToCollect collection.
   *
   * @return all metrics in the metricsToCollect snapshot
   */
  std::vector<prometheus_client::MetricFamily> Collect() const override;

private:
  sdk::metrics::MetricReader *reader_;

  /*
   * Lock when operating the metricsToCollect collection
   */
  mutable std::mutex collection_lock_;
};
}  // namespace metrics
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
