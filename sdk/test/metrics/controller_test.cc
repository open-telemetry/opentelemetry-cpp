// Copyright 2021, OpenTelemetry Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifdef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/controller.h"
#  include "opentelemetry/sdk/metrics/meter.h"
#  include "opentelemetry/sdk/metrics/ungrouped_processor.h"

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
