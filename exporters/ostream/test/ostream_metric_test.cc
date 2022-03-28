// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#ifndef ENABLE_METRICS_PREVIEW

#  include "opentelemetry/exporters/ostream/metric_exporter.h"
#  include "opentelemetry/sdk/metrics/aggregation/default_aggregation.h"
#  include "opentelemetry/sdk/metrics/aggregation/histogram_aggregation.h"

#  include <iostream>

namespace metric_sdk      = opentelemetry::sdk::metrics;
namespace nostd           = opentelemetry::nostd;
namespace exportermetrics = opentelemetry::exporter::metrics;

TEST(OStreamMetricsExporter, Shutdown)
{
  auto exporter =
      std::unique_ptr<metric_sdk::MetricExporter>(new exportermetrics::OStreamMetricExporter);
  ASSERT_TRUE(exporter->Shutdown());
  auto result = exporter->Export(nostd::span<std::unique_ptr<metric_sdk::MetricData>>{});
  EXPECT_EQ(result, opentelemetry::sdk::common::ExportResult::kFailure);
}

TEST(OStreamMetricsExporter, ExportSumPointData)
{
  auto exporter =
      std::unique_ptr<metric_sdk::MetricExporter>(new exportermetrics::OStreamMetricExporter);

  std::unique_ptr<metric_sdk::MetricData> record(new metric_sdk::MetricData);
  auto instrumentation_library =
      opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary::Create("library_name",
                                                                                 "1.2.0");
  record->instrumentation_library_ = instrumentation_library.get();
  record->point_data_              = metric_sdk::SumPointData{
      opentelemetry::common::SystemTimestamp{}, opentelemetry::common::SystemTimestamp{}, 10.0,
      metric_sdk::AggregationTemporality::kUnspecified, false};
  auto record2 = std::unique_ptr<metric_sdk::MetricData>(new metric_sdk::MetricData(*record));
  record2->point_data_ = metric_sdk::SumPointData{
      opentelemetry::common::SystemTimestamp{}, opentelemetry::common::SystemTimestamp{}, 20l,
      metric_sdk::AggregationTemporality::kUnspecified, false};
  std::vector<std::unique_ptr<metric_sdk::MetricData>> records;
  records.push_back(std::move(record));
  records.push_back(std::move(record2));

  std::stringstream stdoutOutput;
  std::streambuf *sbuf = std::cout.rdbuf();
  std::cout.rdbuf(stdoutOutput.rdbuf());

  auto result = exporter->Export(records);
  EXPECT_EQ(result, opentelemetry::sdk::common::ExportResult::kSuccess);
  std::cout.rdbuf(sbuf);

  std::string expectedOutput =
      "{\n"
      "  name        : library_name\n"
      "  version     : 1.2.0\n"
      "  type     : SumPointData\n"
      "  start timestamp     : 0\n"
      "  end timestamp     : 0\n"
      "  value     : 10\n"
      "}\n"
      "{\n"
      "  name        : library_name\n"
      "  version     : 1.2.0\n"
      "  type     : SumPointData\n"
      "  start timestamp     : 0\n"
      "  end timestamp     : 0\n"
      "  value     : 20\n"
      "}\n";
  ASSERT_EQ(stdoutOutput.str(), expectedOutput);
}

TEST(OStreamMetricsExporter, ExportHistogramPointData)
{
  auto exporter =
      std::unique_ptr<metric_sdk::MetricExporter>(new exportermetrics::OStreamMetricExporter);

  std::unique_ptr<metric_sdk::MetricData> record(new metric_sdk::MetricData);
  auto instrumentation_library =
      opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary::Create("library_name",
                                                                                 "1.2.0");
  record->instrumentation_library_ = instrumentation_library.get();
  record->point_data_ = metric_sdk::HistogramPointData{opentelemetry::common::SystemTimestamp{},
                                                       std::vector<double>{10.1, 20.2, 30.2},
                                                       900.5,
                                                       {200, 300, 400, 500},
                                                       3};
  auto record2 = std::unique_ptr<metric_sdk::MetricData>(new metric_sdk::MetricData(*record));
  record2->point_data_ = metric_sdk::HistogramPointData{opentelemetry::common::SystemTimestamp{},
                                                        std::vector<long>{10, 20, 30},
                                                        900l,
                                                        {200, 300, 400, 500},
                                                        3};
  std::vector<std::unique_ptr<metric_sdk::MetricData>> records;
  records.push_back(std::move(record));
  records.push_back(std::move(record2));

  std::stringstream stdoutOutput;
  std::streambuf *sbuf = std::cout.rdbuf();
  std::cout.rdbuf(stdoutOutput.rdbuf());

  auto result = exporter->Export(records);
  EXPECT_EQ(result, opentelemetry::sdk::common::ExportResult::kSuccess);
  std::cout.rdbuf(sbuf);

  std::string expectedOutput =
      "{\n"
      "  name        : library_name\n"
      "  version     : 1.2.0\n"
      "  type     : HistogramPointData\n"
      "  timestamp     : 0\n"
      "  count     : 3\n"
      "  sum     : 900.5\n"
      "  buckets     : [10.1, 20.2, 30.2]\n"
      "  counts     : [200, 300, 400, 500]\n"
      "}\n"
      "{\n"
      "  name        : library_name\n"
      "  version     : 1.2.0\n"
      "  type     : HistogramPointData\n"
      "  timestamp     : 0\n"
      "  count     : 3\n"
      "  sum     : 900\n"
      "  buckets     : [10, 20, 30]\n"
      "  counts     : [200, 300, 400, 500]\n"
      "}\n";
  ASSERT_EQ(stdoutOutput.str(), expectedOutput);
}

TEST(OStreamMetricsExporter, ExportLastValuePointData)
{
  auto exporter =
      std::unique_ptr<metric_sdk::MetricExporter>(new exportermetrics::OStreamMetricExporter);

  std::unique_ptr<metric_sdk::MetricData> record(new metric_sdk::MetricData);
  auto instrumentation_library =
      opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary::Create("library_name",
                                                                                 "1.2.0");
  record->instrumentation_library_ = instrumentation_library.get();
  record->point_data_ =
      metric_sdk::LastValuePointData{opentelemetry::common::SystemTimestamp{}, true, 10.0};
  auto record2 = std::unique_ptr<metric_sdk::MetricData>(new metric_sdk::MetricData(*record));
  record2->point_data_ =
      metric_sdk::LastValuePointData{opentelemetry::common::SystemTimestamp{}, true, 20l};
  std::vector<std::unique_ptr<metric_sdk::MetricData>> records;
  records.push_back(std::move(record));
  records.push_back(std::move(record2));

  std::stringstream stdoutOutput;
  std::streambuf *sbuf = std::cout.rdbuf();
  std::cout.rdbuf(stdoutOutput.rdbuf());

  auto result = exporter->Export(records);
  EXPECT_EQ(result, opentelemetry::sdk::common::ExportResult::kSuccess);
  std::cout.rdbuf(sbuf);

  std::string expectedOutput =
      "{\n"
      "  name        : library_name\n"
      "  version     : 1.2.0\n"
      "  type     : LastValuePointData\n"
      "  timestamp     : 0\n"
      "  valid     : true\n"
      "  value     : 10\n"
      "}\n"
      "{\n"
      "  name        : library_name\n"
      "  version     : 1.2.0\n"
      "  type     : LastValuePointData\n"
      "  timestamp     : 0\n"
      "  valid     : true\n"
      "  value     : 20\n"
      "}\n";
  ASSERT_EQ(stdoutOutput.str(), expectedOutput);
}

TEST(OStreamMetricsExporter, ExportDropPointData)
{
  auto exporter =
      std::unique_ptr<metric_sdk::MetricExporter>(new exportermetrics::OStreamMetricExporter);

  std::unique_ptr<metric_sdk::MetricData> record(new metric_sdk::MetricData);
  auto instrumentation_library =
      opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary::Create("library_name",
                                                                                 "1.2.0");
  record->instrumentation_library_ = instrumentation_library.get();
  record->point_data_              = metric_sdk::DropPointData{};
  std::vector<std::unique_ptr<metric_sdk::MetricData>> records;
  records.push_back(std::move(record));

  std::stringstream stdoutOutput;
  std::streambuf *sbuf = std::cout.rdbuf();
  std::cout.rdbuf(stdoutOutput.rdbuf());

  auto result = exporter->Export(records);
  EXPECT_EQ(result, opentelemetry::sdk::common::ExportResult::kSuccess);
  std::cout.rdbuf(sbuf);

  std::string expectedOutput =
      "{\n"
      "  name        : library_name\n"
      "  version     : 1.2.0\n"
      "}\n";

  ASSERT_EQ(stdoutOutput.str(), expectedOutput);
}

#endif
