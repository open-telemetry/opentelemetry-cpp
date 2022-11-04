// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0/

#pragma once

#ifdef ENABLE_LOGS_PREVIEW

#  include <memory>
#  include <vector>

#  include "opentelemetry/logs/logger_provider.h"
#  include "opentelemetry/nostd/shared_ptr.h"
#  include "opentelemetry/sdk/common/atomic_shared_ptr.h"
#  include "opentelemetry/sdk/logs/logger.h"
#  include "opentelemetry/sdk/logs/logger_context.h"
#  include "opentelemetry/sdk/logs/processor.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{

/**
 * Factory class for LoggerProvider.
 */
class LoggerProviderFactory
{
public:
  /**
   * Create a LoggerProvider.
   */
  static std::unique_ptr<opentelemetry::logs::LoggerProvider> Create(
      std::unique_ptr<LogRecordProcessor> &&processor);

  /**
   * Create a LoggerProvider.
   */
  static std::unique_ptr<opentelemetry::logs::LoggerProvider> Create(
      std::unique_ptr<LogRecordProcessor> &&processor,
      const opentelemetry::sdk::resource::Resource &resource);

  /**
   * Create a LoggerProvider.
   */
  static std::unique_ptr<opentelemetry::logs::LoggerProvider> Create(
      std::vector<std::unique_ptr<LogRecordProcessor>> &&processors);

  /**
   * Create a LoggerProvider.
   */
  static std::unique_ptr<opentelemetry::logs::LoggerProvider> Create(
      std::vector<std::unique_ptr<LogRecordProcessor>> &&processors,
      const opentelemetry::sdk::resource::Resource &resource);

  /**
   * Create a LoggerProvider.
   */
  static std::unique_ptr<opentelemetry::logs::LoggerProvider> Create(
      std::shared_ptr<sdk::logs::LoggerContext> context);
};

}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif /* ENABLE_LOGS_PREVIEW */
