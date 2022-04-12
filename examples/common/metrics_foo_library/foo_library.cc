// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include <chrono>
#  include <thread>
#  include "opentelemetry/metrics/provider.h"

namespace nostd       = opentelemetry::nostd;
namespace metrics_api = opentelemetry::metrics;

void foo_library(const std::string &name)
{
  // Get the Meter from the MeterProvider
  auto provider                               = metrics_api::Provider::GetMeterProvider();
  nostd::shared_ptr<metrics_api::Meter> meter = provider->GetMeter(name, "1.2.0");
  auto double_counter                         = meter->CreateDoubleCounter(name);
  double_counter->Add(28.5);
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  double_counter->Add(3.14);
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  double_counter->Add(23.5);
  std::this_thread::sleep_for(std::chrono::milliseconds(5000));
}
#endif
