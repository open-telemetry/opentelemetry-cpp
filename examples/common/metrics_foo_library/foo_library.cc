// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "foo_library.h"
#  include <chrono>
#  include <list>
#  include <thread>
#  include "opentelemetry/metrics/provider.h"

namespace nostd       = opentelemetry::nostd;
namespace metrics_api = opentelemetry::metrics;

void foo_library::counter_example(const std::string &name)
{
  auto provider                               = metrics_api::Provider::GetMeterProvider();
  nostd::shared_ptr<metrics_api::Meter> meter = provider->GetMeter(name, "1.2.0");
  auto double_counter                         = meter->CreateDoubleCounter(name);

  while (true)
  {
    double val = (rand() % 700) + 1.1;
    double_counter->Add(val);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }
}

void foo_library::histogram_example(const std::string &name)
{
  auto provider                               = metrics_api::Provider::GetMeterProvider();
  nostd::shared_ptr<metrics_api::Meter> meter = provider->GetMeter(name, "1.2.0");
  auto histogram_counter                      = meter->CreateDoubleHistogram(name);
  auto context                                = opentelemetry::context::Context{};
  std::map<std::string, std::string> labels   = {{"key", "value"}};
  auto labelkv = opentelemetry::common::KeyValueIterableView<decltype(labels)>{labels};
  while (true)
  {
    double val = (rand() % 700) + 1.1;
    histogram_counter->Record(val, labelkv, context);
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
  }
}

#endif
