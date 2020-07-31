#include "opentelemetry/sdk/metrics/controller.h"
#include "opentelemetry/sdk/metrics/meter.h"
#include "opentelemetry/sdk/metrics/ungrouped_processor.h"

#include <gtest/gtest.h>
#include <thread>
#include <numeric>
// #include <chrono>

namespace metrics_api = opentelemetry::metrics;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

TEST(Controller, Constructor)
{
    
    std::shared_ptr<metrics_api::Meter> meter = std::shared_ptr<metrics_api::Meter>(new Meter("Test"));
    PushController alpha(meter,
                         std::unique_ptr<MetricsExporter>(new opentelemetry::exporter::metrics::OStreamMetricsExporter),
                         std::shared_ptr<MetricsProcessor>(new opentelemetry::sdk::metrics::UngroupedMetricsProcessor(false)),
                         .05);
    
    auto instr = meter->NewIntCounter("test","none","none",true);
    std::map<std::string, std::string> labels = {{"key", "value"}};
    auto labelkv = trace::KeyValueIterableView<decltype(labels)>{labels};
    
    alpha.start();
    for (int i=0; i<20; i++){
        instr->add(i, labelkv);
        usleep(.01*1000000);
    }
    alpha.stop();
}


}  // namespace sdk
}  // namespace metrics
OPENTELEMETRY_END_NAMESPACE
