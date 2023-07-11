// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/logs/logger_provider_factory.h"
#include "opentelemetry/sdk/logs/logger_context.h"
#include "opentelemetry/sdk/logs/logger_provider.h"
#include "opentelemetry/sdk/resource/resource.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{

std::unique_ptr<opentelemetry::logs::LoggerProvider> LoggerProviderFactory::Create(
    std::unique_ptr<LogRecordProcessor> &&processor)
{
  auto resource = opentelemetry::sdk::resource::Resource::Create({});
  return Create(std::move(processor), resource);
}

std::unique_ptr<opentelemetry::logs::LoggerProvider> LoggerProviderFactory::Create(
    std::unique_ptr<LogRecordProcessor> &&processor,
    const opentelemetry::sdk::resource::Resource &resource)
{
  std::unique_ptr<opentelemetry::logs::LoggerProvider> provider(
      new LoggerProvider(std::move(processor), resource));
  return provider;
}

std::unique_ptr<opentelemetry::logs::LoggerProvider> LoggerProviderFactory::Create(
    std::vector<std::unique_ptr<LogRecordProcessor>> &&processors)
{
  auto resource = opentelemetry::sdk::resource::Resource::Create({});
  return Create(std::move(processors), resource);
}

std::unique_ptr<opentelemetry::logs::LoggerProvider> LoggerProviderFactory::Create(
    std::vector<std::unique_ptr<LogRecordProcessor>> &&processors,
    const opentelemetry::sdk::resource::Resource &resource)
{
  std::unique_ptr<opentelemetry::logs::LoggerProvider> provider(
      new LoggerProvider(std::move(processors), resource));
  return provider;
}

std::unique_ptr<opentelemetry::logs::LoggerProvider> LoggerProviderFactory::Create(
    std::unique_ptr<LoggerContext> context)
{
  std::unique_ptr<opentelemetry::logs::LoggerProvider> provider(
      new LoggerProvider(std::move(context)));
  return provider;
}

}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
