// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/configuration/registry_factory.h"

#include <memory>

#include "opentelemetry/sdk/configuration/logs_builders.h"
#include "opentelemetry/sdk/configuration/metrics_builders.h"
#include "opentelemetry/sdk/configuration/registry.h"
#include "opentelemetry/sdk/configuration/trace_builders.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

std::unique_ptr<Registry> RegistryFactory::Create()
{
  auto registry = std::make_unique<Registry>();
  RegisterDefaultTraceBuilders(registry.get());
  RegisterDefaultMetricsBuilders(registry.get());
  RegisterDefaultLogsBuilders(registry.get());
  return registry;
}

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
