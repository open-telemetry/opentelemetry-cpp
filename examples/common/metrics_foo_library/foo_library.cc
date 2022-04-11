// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <chrono>
#include <thread>
#include "opentelemetry/metrics/provider.h"
#include "opentelemetry/sdk/metrics/aggregation/default_aggregation.h"
#include "opentelemetry/sdk/metrics/aggregation/histogram_aggregation.h"
#include "opentelemetry/sdk/metrics/export/periodic_exporting_metric_reader.h"
#include "opentelemetry/sdk/metrics/meter.h"
#include "opentelemetry/sdk/metrics/meter_provider.h"
#include "opentelemetry/sdk/version/version.h"

namespace metric_sdk  = opentelemetry::sdk::metrics;
namespace nostd       = opentelemetry::nostd;
namespace common      = opentelemetry::common;
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
