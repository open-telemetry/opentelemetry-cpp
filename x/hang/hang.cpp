#include "otel_sdk.h"

#include <opentelemetry/version.h>
#include <opentelemetry/logs/provider.h>
#include <opentelemetry/metrics/provider.h>

int main(int argc, const char* argv[])
{
  using namespace opentelemetry;

  otel_sdk::instance instance;
  std::unique_ptr<metrics::Counter<double>> counter;

  auto provider = metrics::Provider::GetMeterProvider();
  auto meter{ provider->GetMeter("malkia_test_meter", "1.2.0") };
  std::vector<std::unique_ptr<metrics::Counter<double>>> counters;
  counters.reserve(10);
  for(int i=0; i<10; i++)
  {
    char buf[100];
    sprintf(buf,"counter_%d", i);
    counters.push_back( std::move( meter->CreateDoubleCounter(buf)));
  }
  for(int i=0; i<10; i++)
  {
      counters[i]->Add(1);
  }
  std::this_thread::sleep_for(std::chrono::seconds(2));

  quick_exit(0);

  return 0;
}
