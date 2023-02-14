// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>
#include <mutex>
#include <vector>
#include "opentelemetry/metrics/meter.h"
#include "opentelemetry/metrics/meter_provider.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/sdk/metrics/meter.h"
#include "opentelemetry/sdk/metrics/meter_context.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

class MeterProviderFactory
{
public:
  static std::unique_ptr<opentelemetry::metrics::MeterProvider> Create();

  static std::unique_ptr<opentelemetry::metrics::MeterProvider> Create(
      std::unique_ptr<ViewRegistry> views);

  static std::unique_ptr<opentelemetry::metrics::MeterProvider> Create(
      std::unique_ptr<ViewRegistry> views,
      const opentelemetry::sdk::resource::Resource &resource);

  static std::unique_ptr<opentelemetry::metrics::MeterProvider> Create(
      std::shared_ptr<sdk::metrics::MeterContext> context);
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
