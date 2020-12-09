#include "opentelemetry/sdk/metrics/controller.h"
#include "opentelemetry/sdk/metrics/meter.h"
#include "opentelemetry/sdk/metrics/ungrouped_processor.h"

#include <gtest/gtest.h>
#include <numeric>
#include <thread>
// #include <chrono>

namespace metrics_api = opentelemetry::metrics;

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
  auto labelkv = common::KeyValueIterableView<decltype(labels)>{labels};

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
