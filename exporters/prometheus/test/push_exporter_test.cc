// Copyright 2022, OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>

#include <chrono>
#include <cstddef>
#include <memory>
#include <thread>

#include "opentelemetry/version.h"

#include "opentelemetry/exporters/prometheus/collector.h"

#include "opentelemetry/exporters/prometheus/push_exporter.h"
#include "opentelemetry/exporters/prometheus/push_exporter_factory.h"
#include "opentelemetry/exporters/prometheus/push_exporter_options.h"
#include "prometheus_push_test_helper.h"

using opentelemetry::exporter::metrics::PrometheusCollector;
using opentelemetry::exporter::metrics::PrometheusPushExporter;
using opentelemetry::exporter::metrics::PrometheusPushExporterFactory;
using opentelemetry::exporter::metrics::PrometheusPushExporterOptions;
using opentelemetry::sdk::common::ExportResult;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace metrics
{
class PrometheusPushExporterTest
{  // : public ::testing::Test
public:
  PrometheusPushExporter GetExporter() { return PrometheusPushExporter(); }

  void CheckFactory(PrometheusPushExporter &exporter, const PrometheusPushExporterOptions &options)
  {
    ASSERT_EQ(exporter.options_.host, options.host);
    ASSERT_EQ(exporter.options_.port, options.port);
    ASSERT_EQ(exporter.options_.jobname, options.jobname);
    ASSERT_EQ(exporter.options_.username, options.username);
    ASSERT_EQ(exporter.options_.password, options.password);
    ASSERT_EQ(exporter.options_.max_collection_size, options.max_collection_size);
    ASSERT_EQ(exporter.options_.labels.size(), options.labels.size());
  }
};
}  // namespace metrics
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE

using opentelemetry::exporter::metrics::PrometheusPushExporterTest;

/**
 * When a PrometheusPushExporter is initialized,
 * isShutdown should be false.
 */
TEST(PrometheusPushExporter, InitializeConstructorIsNotShutdown)
{
  PrometheusPushExporterTest p;
  PrometheusPushExporter exporter = p.GetExporter();

  // // Asserts that the exporter is not shutdown.
  ASSERT_TRUE(!exporter.IsShutdown());
}

/**
 * The shutdown() function should set the isShutdown field to true.
 */
TEST(PrometheusPushExporter, ShutdownSetsIsShutdownToTrue)
{
  PrometheusPushExporterTest p;
  PrometheusPushExporter exporter = p.GetExporter();

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
TEST(PrometheusPushExporter, ExportSuccessfully)
{
  PrometheusPushExporterTest p;
  PrometheusPushExporter exporter = p.GetExporter();

  auto res = exporter.Export(CreateSumPointData());

  // result should be kSuccess = 0
  ExportResult code = ExportResult::kSuccess;
  ASSERT_EQ(res, code);
}

/**
 * If the exporter is shutdown, it cannot process
 * any more export requests and returns kFailure = 1.
 */
TEST(PrometheusPushExporter, ExporterIsShutdown)
{
  PrometheusPushExporterTest p;
  PrometheusPushExporter exporter = p.GetExporter();

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
TEST(PrometheusPushExporter, CollectionNotEnoughSpace)
{
  PrometheusPushExporterTest p;
  PrometheusPushExporter exporter = p.GetExporter();

  // prepare two collections of records to export,
  // one close to max size and another one that, when added
  // to the first, will exceed the size of the collection

  int max_collection_size = exporter.GetMaxCollectionSize();

  // send export request to fill the
  // collection in the collector
  ExportResult code = ExportResult::kSuccess;
  for (int count = 1; count < max_collection_size; ++count)
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
TEST(PrometheusPushExporter, InvalidArgumentWhenPassedEmptyRecordCollection)
{
  PrometheusPushExporterTest p;
  PrometheusPushExporter exporter = p.GetExporter();

  // Initializes an empty colelction of records
  metric_sdk::ResourceMetrics data;

  // send export request to fill the
  // collection in the collector
  auto res = exporter.Export(data);

  // the result code should be kFailureInvalidArgument = 3
  ExportResult code = ExportResult::kFailureInvalidArgument;
  ASSERT_EQ(res, code);
}

TEST(PrometheusPushExporterFactory, Create)
{
  PrometheusPushExporterOptions options;
  options.host                 = "localhost";
  options.port                 = std::to_string(4138);
  options.jobname              = "jobname";
  options.labels["test_label"] = "test_value";
  options.username             = "user";
  options.password             = "pawword";
  options.max_collection_size  = 2345;

  PrometheusPushExporterTest p;
  auto exporter = PrometheusPushExporterFactory::Create(options);
  ASSERT_TRUE(!!exporter);

  ASSERT_EQ(static_cast<PrometheusPushExporter *>(exporter.get())->GetMaxCollectionSize(),
            options.max_collection_size);

  p.CheckFactory(*static_cast<PrometheusPushExporter *>(exporter.get()), options);
}
