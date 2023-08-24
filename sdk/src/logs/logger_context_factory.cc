// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/logs/logger_context_factory.h"
#include "opentelemetry/sdk/logs/logger_context.h"
#include "opentelemetry/sdk/logs/processor.h"
#include "opentelemetry/sdk/resource/resource.h"

#include <memory>
#include <vector>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{

std::unique_ptr<LoggerContext> LoggerContextFactory::Create(
    std::vector<std::unique_ptr<LogRecordProcessor>> &&processors)
{
  auto resource = opentelemetry::sdk::resource::Resource::Create({});
  return Create(std::move(processors), resource);
}

std::unique_ptr<LoggerContext> LoggerContextFactory::Create(
    std::vector<std::unique_ptr<LogRecordProcessor>> &&processors,
    const opentelemetry::sdk::resource::Resource &resource)
{
  std::unique_ptr<LoggerContext> context(new LoggerContext(std::move(processors), resource));
  return context;
}

}  // namespace logs
}  // namespace sdk

OPENTELEMETRY_END_NAMESPACE
