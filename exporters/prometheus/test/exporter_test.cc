// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>

#include "opentelemetry/exporters/prometheus/collector.h"
#include "opentelemetry/exporters/prometheus/exporter.h"
#include "opentelemetry/version.h"
#include "prometheus_test_helper.h"

/**
 * PrometheusExporterTest is a friend class of PrometheusExporter.
 * It has access to a private constructor that does not take in
 * an exposer as an argument, and instead takes no arguments; this
 * private constructor is only to be used here for testing
 */
OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace metrics
{
class PrometheusExporterTest  // : public ::testing::Test
{
public:
  PrometheusExporter GetExporter() { return PrometheusExporter(); }
};
}  // namespace metrics
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE

using opentelemetry::exporter::metrics::PrometheusCollector;
using opentelemetry::exporter::metrics::PrometheusExporter;
using opentelemetry::exporter::metrics::PrometheusExporterTest;
using opentelemetry::sdk::common::ExportResult;

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

  auto res = exporter.Export(CreateSumPointData());

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

  exporter.Shutdown();

  // send export request after shutdown
  auto res = exporter.Export(CreateSumPointData());

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

  // prepare two collections of records to export,
  // one close to max size and another one that, when added
  // to the first, will exceed the size of the collection

  int max_collection_size = exporter.GetCollector()->GetMaxCollectionSize();

  // send export request to fill the
  // collection in the collector
  ExportResult code = ExportResult::kSuccess;
  for (int count = 1; count <= max_collection_size; ++count)
  {
    auto res = exporter.Export(CreateSumPointData());
    ASSERT_EQ(res, code);
  }

  // send export request that does not complete
  // due to not enough space in the collection
  auto res = exporter.Export(CreateSumPointData());

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
  metric_sdk::ResourceMetrics data;

  // send export request to fill the
  // collection in the collector
  auto res = exporter.Export(data);

  // the result code should be kFailureInvalidArgument = 3
  ExportResult code = ExportResult::kFailureInvalidArgument;
  ASSERT_EQ(res, code);
}
