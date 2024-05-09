// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0/

#pragma once

#include <memory>
#include <vector>

#include "opentelemetry/sdk/logs/logger_provider.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace resource
{
class Resource;
}  // namespace resource

namespace logs
{
class LoggerContext;
class LogRecordProcessor;

/**
 * Factory class for LoggerProvider.
 */
class OPENTELEMETRY_EXPORT LoggerProviderFactory
{
public:
  /**
   * Create a LoggerProvider.
   */
  static std::unique_ptr<opentelemetry::sdk::logs::LoggerProvider> Create(
      std::unique_ptr<LogRecordProcessor> &&processor);

  /**
   * Create a LoggerProvider.
   */
  static std::unique_ptr<opentelemetry::sdk::logs::LoggerProvider> Create(
      std::unique_ptr<LogRecordProcessor> &&processor,
      const opentelemetry::sdk::resource::Resource &resource);

  /**
   * Create a LoggerProvider.
   */
  static std::unique_ptr<opentelemetry::sdk::logs::LoggerProvider> Create(
      std::vector<std::unique_ptr<LogRecordProcessor>> &&processors);

  /**
   * Create a LoggerProvider.
   */
  static std::unique_ptr<opentelemetry::sdk::logs::LoggerProvider> Create(
      std::vector<std::unique_ptr<LogRecordProcessor>> &&processors,
      const opentelemetry::sdk::resource::Resource &resource);

  /**
   * Create a LoggerProvider.
   */
  static std::unique_ptr<opentelemetry::sdk::logs::LoggerProvider> Create(
      std::unique_ptr<LoggerContext> context);
};

}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
