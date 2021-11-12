/*
 * Copyright The OpenTelemetry Authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifdef ENABLE_METRICS_PREVIEW

#  include <gtest/gtest.h>
#  include <typeinfo>

#  include "opentelemetry/exporters/prometheus/prometheus_collector.h"
#  include "opentelemetry/exporters/prometheus/prometheus_exporter.h"
#  include "opentelemetry/sdk/_metrics/aggregator/counter_aggregator.h"
#  include "opentelemetry/version.h"

/**
 * PrometheusExporterTest is a friend class of PrometheusExporter.
 * It has access to a private constructor that does not take in
 * an exposer as an argument, and instead takes no arguments; this
 * private constructor is only to be used here for testing
 */
OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace prometheus
{
class PrometheusExporterTest  // : public ::testing::Test
{
public:
  PrometheusExporter GetExporter() { return PrometheusExporter(); }
};
}  // namespace prometheus
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE

using opentelemetry::exporter::prometheus::PrometheusCollector;
using opentelemetry::exporter::prometheus::PrometheusExporter;
using opentelemetry::exporter::prometheus::PrometheusExporterTest;
using opentelemetry::sdk::common::ExportResult;
using opentelemetry::sdk::metrics::CounterAggregator;
using opentelemetry::sdk::metrics::Record;

/**
 * Helper function to create a collection of records taken from
 * a counter aggregator
 */
std::vector<Record> CreateRecords(int num)
{

  std::vector<Record> records;

  for (int i = 0; i < num; i++)
  {
    std::string name        = "record-" + std::to_string(i);
    std::string description = "record-" + std::to_string(i);
    std::string labels      = "record-" + std::to_string(i) + "-label-1.0";
    auto aggregator         = std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<int>>(
        new opentelemetry::sdk::metrics::CounterAggregator<int>(
            opentelemetry::metrics::InstrumentKind::Counter));
    aggregator->update(10);
    aggregator->checkpoint();

    Record r{name, description, labels, aggregator};
    records.push_back(r);
  }
  return records;
}

/**
 * When a PrometheusExporter is initialized,
 * isShutdown should be false.
 */
TEST(PrometheusExporter, InitializeConstructorIsNotShutdown)
{
  PrometheusExporterTest p;
  PrometheusExporter exporter = p.GetExporter();

  // // Asserts that the exporter is not shutdown.
  ASSERT_TRUE(!exporter.IsShutdown());
}

/**
 * The shutdown() function should set the isShutdown field to true.
 */
TEST(PrometheusExporter, ShutdownSetsIsShutdownToTrue)
{
  PrometheusExporterTest p;
  PrometheusExporter exporter = p.GetExporter();

  // exporter shuold not be shutdown by default
  ASSERT_TRUE(!exporter.IsShutdown());

  exporter.Shutdown();

  // the exporter shuold be shutdown
  ASSERT_TRUE(exporter.IsShutdown());

  // shutdown function should be idempotent
  exporter.Shutdown();
  ASSERT_TRUE(exporter.IsShutdown());
}

/**
 * The Export() function should return kSuccess = 0
 *  when data is exported successfully.
 */
TEST(PrometheusExporter, ExportSuccessfully)
{
  PrometheusExporterTest p;
  PrometheusExporter exporter = p.GetExporter();

  int num_records = 2;

  std::vector<Record> records = CreateRecords(num_records);

  auto res = exporter.Export(records);

  // result should be kSuccess = 0
  ExportResult code = ExportResult::kSuccess;
  ASSERT_EQ(res, code);
}

/**
 * If the exporter is shutdown, it cannot process
 * any more export requests and returns kFailure = 1.
 */
TEST(PrometheusExporter, ExporterIsShutdown)
{
  PrometheusExporterTest p;
  PrometheusExporter exporter = p.GetExporter();

  int num_records = 1;

  std::vector<Record> records = CreateRecords(num_records);

  exporter.Shutdown();

  // send export request after shutdown
  auto res = exporter.Export(records);

  // result code should be kFailure = 1
  ExportResult code = ExportResult::kFailure;
  ASSERT_EQ(res, code);
}

/**
 * The Export() function should return
 * kFailureFull = 2 when the collection is full,
 * or when the collection is not full but does not have enough
 * space to hold the batch data.
 */
TEST(PrometheusExporter, CollectionNotEnoughSpace)
{
  PrometheusExporterTest p;
  PrometheusExporter exporter = p.GetExporter();

  int num_records = 2;

  // prepare two collections of records to export,
  // one close to max size and another one that, when added
  // to the first, will exceed the size of the collection

  int max_collection_size = exporter.GetCollector()->GetMaxCollectionSize();

  std::vector<Record> full_records = CreateRecords(max_collection_size - 1);
  std::vector<Record> records      = CreateRecords(num_records);

  // send export request to fill the
  // collection in the collector
  auto res = exporter.Export(full_records);

  // the result code should be kSuccess = 0
  ExportResult code = ExportResult::kSuccess;
  ASSERT_EQ(res, code);

  // send export request that does not complete
  // due to not enough space in the collection
  res = exporter.Export(records);

  // the result code should be kFailureFull = 2
  code = ExportResult::kFailureFull;
  ASSERT_EQ(res, code);
}

/**
 *  The Export() function should return
 *  kFailureInvalidArgument = 3 when an empty collection
 *  of records is passed to the Export() function.
 */
TEST(PrometheusExporter, InvalidArgumentWhenPassedEmptyRecordCollection)
{
  PrometheusExporterTest p;
  PrometheusExporter exporter = p.GetExporter();

  // Initializes an empty colelction of records
  std::vector<Record> records;

  // send export request to fill the
  // collection in the collector
  auto res = exporter.Export(records);

  // the result code should be kFailureInvalidArgument = 3
  ExportResult code = ExportResult::kFailureInvalidArgument;
  ASSERT_EQ(res, code);
}

#endif  // ENABLE_METRICS_PREVIEW
