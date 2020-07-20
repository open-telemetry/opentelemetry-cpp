#include "opentelemetry/sdk/metrics/exporter.h"
#include "opentelemetry/exporters/stream/metrics_exporter.h"
#include "opentelemetry/sdk/metrics/record.h"

#include <iostream>
#include <gtest/gtest.h>

namespace sdkmetrics = opentelemetry::sdk::metrics;

TEST(StreamMetricsExporter, PrintCounter)
{
  auto exporter = std::unique_ptr<sdkmetrics::MetricsExporter> (new
      opentelemetry::exporter::metrics::StreamMetricsExporter);
  
  std::vector<int> vec (1,5);
  auto record = sdkmetrics::Record("name", "description", AggregatorKind::Counter, "labels", vec);

  std::vector<sdkmetrics::Record> records;
  records.push_back(record);

  // Create stringstream to redirect to
  std::stringstream stdoutOutput;

  // Save cout's buffer here
  std::streambuf *sbuf = std::cout.rdbuf();

  // Redirect cout to our stringstream buffer
  std::cout.rdbuf(stdoutOutput.rdbuf());

  exporter->Export(records);

  std::cout.rdbuf(sbuf);

  std::string expectedOutput = 
  "{\n"
  "  name        : name\n"
  "  description : description\n"
  "  labels      : labels\n"
  "  sum         : 5\n"
  "}\n"; 

  ASSERT_EQ(stdoutOutput.str(),expectedOutput);
}

TEST(StreamMetricsExporter, PrintMinMaxSumCount)
{
  auto exporter = std::unique_ptr<sdkmetrics::MetricsExporter> (new
      opentelemetry::exporter::metrics::StreamMetricsExporter);
  
  std::vector<int> vec;
  vec.push_back(1);
  vec.push_back(2);
  vec.push_back(3);
  vec.push_back(2);

  auto record = sdkmetrics::Record("name", "description", AggregatorKind::MinMaxSumCount, "labels", vec);

  std::vector<sdkmetrics::Record> records;
  records.push_back(record);

  // Create stringstream to redirect to
  std::stringstream stdoutOutput;

  // Save cout's buffer here
  std::streambuf *sbuf = std::cout.rdbuf();

  // Redirect cout to our stringstream buffer
  std::cout.rdbuf(stdoutOutput.rdbuf());

  exporter->Export(records);

  std::cout.rdbuf(sbuf);

  std::string expectedOutput = 
  "{\n"
  "  name        : name\n"
  "  description : description\n"
  "  labels      : labels\n"
  "  min         : 1\n"
  "  max         : 2\n"
  "  sum         : 3\n"
  "  count       : 2\n"
  "}\n"; 

  ASSERT_EQ(stdoutOutput.str(),expectedOutput);
}