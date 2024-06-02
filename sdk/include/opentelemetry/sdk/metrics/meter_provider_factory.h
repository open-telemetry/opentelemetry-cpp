// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "opentelemetry/metrics/meter_provider.h"
#include "opentelemetry/sdk/metrics/meter_context.h"
#include "opentelemetry/sdk/metrics/meter_provider.h"
#include "opentelemetry/sdk/metrics/view/view_registry.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

class OPENTELEMETRY_EXPORT MeterProviderFactory
{
public:

#ifdef OPENTELEMETRY_DEPRECATED_SDK_FACTORY

#ifndef OPENTELEMETRY_NO_DEPRECATED_CODE

  OPENTELEMETRY_DEPRECATED
  static std::unique_ptr<opentelemetry::metrics::MeterProvider> Create();

  OPENTELEMETRY_DEPRECATED
  static std::unique_ptr<opentelemetry::metrics::MeterProvider> Create(
      std::unique_ptr<ViewRegistry> views);

  OPENTELEMETRY_DEPRECATED
  static std::unique_ptr<opentelemetry::metrics::MeterProvider> Create(
      std::unique_ptr<ViewRegistry> views,
      const opentelemetry::sdk::resource::Resource &resource);

  OPENTELEMETRY_DEPRECATED
  static std::unique_ptr<opentelemetry::metrics::MeterProvider> Create(
      std::unique_ptr<sdk::metrics::MeterContext> context);

#endif /* OPENTELEMETRY_NO_DEPRECATED_CODE */

#else

  static std::unique_ptr<opentelemetry::sdk::metrics::MeterProvider> Create();

  static std::unique_ptr<opentelemetry::sdk::metrics::MeterProvider> Create(
      std::unique_ptr<ViewRegistry> views);

  static std::unique_ptr<opentelemetry::sdk::metrics::MeterProvider> Create(
      std::unique_ptr<ViewRegistry> views,
      const opentelemetry::sdk::resource::Resource &resource);

  static std::unique_ptr<opentelemetry::sdk::metrics::MeterProvider> Create(
      std::unique_ptr<sdk::metrics::MeterContext> context);

#endif /* OPENTELEMETRY_DEPRECATED_SDK_FACTORY */

};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
