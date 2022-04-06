// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW

#  include "opentelemetry/sdk/metrics/export/periodic_exporting_metric_reader.h"
#  include "opentelemetry/sdk/metrics/export/metric_producer.h"
#  include "opentelemetry/sdk/metrics/metric_exporter.h"

#  include <gtest/gtest.h>

using namespace opentelemetry;
using namespace opentelemetry::sdk::instrumentationlibrary;
using namespace opentelemetry::sdk::metrics;

class MockPushMetricExporter : public MetricExporter
{
public:
  opentelemetry::sdk::common::ExportResult Export(const ResourceMetrics &record) noexcept override
  {
    records_.push_back(record);
    return opentelemetry::sdk::common::ExportResult::kSuccess;
  }

  bool ForceFlush(
      std::chrono::microseconds timeout = (std::chrono::microseconds::max)()) noexcept override
  {
    return false;
  }

  bool Shutdown(std::chrono::microseconds timeout = std::chrono::microseconds(0)) noexcept override
  {
    return true;
  }

  size_t GetDataCount() { return records_.size(); }

private:
  std::vector<ResourceMetrics> records_;
};

class MockMetricProducer : public MetricProducer
{
public:
  MockMetricProducer(std::chrono::microseconds sleep_ms = std::chrono::microseconds::zero())
      : sleep_ms_{sleep_ms}, data_sent_size_(0)
  {}

  bool Collect(nostd::function_ref<bool(ResourceMetrics &)> callback) noexcept override
  {
    std::this_thread::sleep_for(sleep_ms_);
    data_sent_size_++;
    ResourceMetrics data;
    callback(data);
    return true;
  }

  size_t GetDataCount() { return data_sent_size_; }

private:
  std::chrono::microseconds sleep_ms_;
  size_t data_sent_size_;
};

TEST(PeriodicExporingMetricReader, BasicTests)
{
  std::unique_ptr<MetricExporter> exporter(new MockPushMetricExporter());
  PeriodicExportingMetricReaderOptions options;
  options.export_timeout_millis  = std::chrono::milliseconds(200);
  options.export_interval_millis = std::chrono::milliseconds(500);
  auto exporter_ptr              = exporter.get();
  PeriodicExportingMetricReader reader(std::move(exporter), options);
  MockMetricProducer producer;
  reader.SetMetricProducer(&producer);
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));
  reader.Shutdown();
  EXPECT_EQ(static_cast<MockPushMetricExporter *>(exporter_ptr)->GetDataCount(),
            static_cast<MockMetricProducer *>(&producer)->GetDataCount());
}

#endif