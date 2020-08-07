#include <gtest/gtest.h>
#include <typeinfo>

#include "opentelemetry/exporters/prometheus/prometheus_collector.h"
#include "opentelemetry/exporters/prometheus/prometheus_exporter.h"
#include "opentelemetry/sdk/metrics/aggregator/counter_aggregator.h"
#include "opentelemetry/version.h"

/**
 * PrometheusExporterTest is a friend class of PrometheusExporter.
 * It has access to a private constructor that does not take in
 * an exposer as an argument, and instead takes no arguments; this
 * private constructor is only to be used here for testing
 */
class opentelemetry::exporter::prometheus::PrometheusExporterTest
{
public:
  opentelemetry::exporter::prometheus::PrometheusExporter GetExporter()
  {
    return opentelemetry::exporter::prometheus::PrometheusExporter();
  }
};

using opentelemetry::exporter::prometheus::PrometheusCollector;
using opentelemetry::exporter::prometheus::PrometheusExporter;
using opentelemetry::exporter::prometheus::PrometheusExporterTest;
using opentelemetry::sdk::metrics::CounterAggregator;
using opentelemetry::sdk::metrics::Record;
using opentelemetry::sdk::metrics::ExportResult;

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

  // Asserts that the exporter is not shutdown.
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
 * The Export() function should return SUCCESS = 0
 *  when data is exported successfully.
 */
TEST(PrometheusExporter, ExportSuccessfully)
{
  PrometheusExporterTest p;
  PrometheusExporter exporter = p.GetExporter();

  int num_records = 2;

  std::vector<Record> records = CreateRecords(num_records);

  auto res = exporter.Export(records);

  // result should be SUCCESS = 0
  ExportResult code = ExportResult::kSuccess;
  ASSERT_EQ(res, code);
}

/**
 * If the exporter is shutdown, it cannot process
 * any more export requests and returns FAILURE_ALREADY_SHUTDOWN = 4.
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

  // result code should be FAILURE_ALREADY_SHUTDOWN = 4
  ExportResult code = ExportResult::kFailure;
  ASSERT_EQ(res, code);
}

/**
 * The Export() function should return
 * FAILURE_FULL_COLLECTION = 1 when the collection is full,
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

  // the result code should be SUCCESS = 0
  ExportResult code = ExportResult::kSuccess;
  ASSERT_EQ(res, code);

  // send export request that does not complete
  // due to not enough space in the collection
  res = exporter.Export(records);

  // the result code should be FAILURE_FULL_COLLECTION = 1
  code = ExportResult::kFailureFull;
  ASSERT_EQ(res, code);
}

/**
 *  The Export() function should return
 *  FAILURE_INVALID_ARGUMENT = 3 when an empty collection
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

  // the result code should be FAILURE_INVALID_ARGUMENT = 3
  ExportResult code = ExportResult::kFailureInvalidArgument;
  ASSERT_EQ(res, code);
}
