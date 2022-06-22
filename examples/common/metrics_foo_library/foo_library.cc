// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "foo_library.h"
#  include <chrono>
#  include <map>
#  include <thread>
#  include <vector>
#  include "opentelemetry/context/context.h"
#  include "opentelemetry/metrics/provider.h"

namespace nostd       = opentelemetry::nostd;
namespace metrics_api = opentelemetry::metrics;

namespace
{

std::map<std::string, std::string> get_random_attr()
{
  static const std::vector<std::pair<std::string, std::string>> labels = {{"key1", "value1"},
                                                                          {"key2", "value2"},
                                                                          {"key3", "value3"},
                                                                          {"key4", "value4"},
                                                                          {"key5", "value5"}};
  return std::map<std::string, std::string>{labels[rand() % (labels.size() - 1)],
                                            labels[rand() % (labels.size() - 1)]};
}

class MeasurementFetcher
{
public:
  static void Fetcher(opentelemetry::metrics::ObserverResult<double> &observer_result, void *state)
  {
    double val                                = (rand() % 700) + 1.1;
    std::map<std::string, std::string> labels = get_random_attr();
    auto labelkv = opentelemetry::common::KeyValueIterableView<decltype(labels)>{labels};
    observer_result.Observe(val /*, labelkv*/);
  }
};
}  // namespace

void foo_library::counter_example(const std::string &name)
{
  std::string counter_name                    = name + "_counter";
  auto provider                               = metrics_api::Provider::GetMeterProvider();
  nostd::shared_ptr<metrics_api::Meter> meter = provider->GetMeter(name, "1.2.0");
  auto double_counter                         = meter->CreateDoubleCounter(counter_name);

  while (true)
  {
    double val = (rand() % 700) + 1.1;
    double_counter->Add(val);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }
}

void foo_library::observable_counter_example(const std::string &name)
{
  std::string counter_name                    = name + "_observable_counter";
  auto provider                               = metrics_api::Provider::GetMeterProvider();
  nostd::shared_ptr<metrics_api::Meter> meter = provider->GetMeter(name, "1.2.0");
  meter->CreateDoubleObservableCounter(counter_name, MeasurementFetcher::Fetcher);
  while (true)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }
}

void foo_library::histogram_example(const std::string &name)
{
  std::string histogram_name                  = name + "_histogram";
  auto provider                               = metrics_api::Provider::GetMeterProvider();
  nostd::shared_ptr<metrics_api::Meter> meter = provider->GetMeter(name, "1.2.0");
  auto histogram_counter                      = meter->CreateDoubleHistogram(histogram_name);
  auto context                                = opentelemetry::context::Context{};
  while (true)
  {
    double val                                = (rand() % 700) + 1.1;
    std::map<std::string, std::string> labels = get_random_attr();
    auto labelkv = opentelemetry::common::KeyValueIterableView<decltype(labels)>{labels};
    histogram_counter->Record(val, labelkv, context);
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
  }
}

#endif
