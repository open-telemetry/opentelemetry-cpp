// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/_metrics/controller.h"
#  include "opentelemetry/sdk/_metrics/meter.h"
#  include "opentelemetry/sdk/_metrics/ungrouped_processor.h"

#  include <gtest/gtest.h>
#  include <numeric>
#  include <thread>
// #include <chrono>

namespace metrics_api = opentelemetry::metrics;
using namespace opentelemetry::sdk::common;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
class DummyExporter : public MetricsExporter
{
  ExportResult Export(const std::vector<Record> &records) noexcept
  {
    return ExportResult::kSuccess;
  }
};

TEST(Controller, Constructor)
{

  std::shared_ptr<metrics_api::Meter> meter =
      std::shared_ptr<metrics_api::Meter>(new Meter("Test"));
  PushController alpha(meter, std::unique_ptr<MetricsExporter>(new DummyExporter),
                       std::shared_ptr<MetricsProcessor>(
                           new opentelemetry::sdk::metrics::UngroupedMetricsProcessor(false)),
                       .05);

  auto instr                                = meter->NewIntCounter("test", "none", "none", true);
  std::map<std::string, std::string> labels = {{"key", "value"}};
  auto labelkv = opentelemetry::common::KeyValueIterableView<decltype(labels)>{labels};

  alpha.start();

  for (int i = 0; i < 20; i++)
  {
    instr->add(i, labelkv);
  }
  alpha.stop();
}

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
