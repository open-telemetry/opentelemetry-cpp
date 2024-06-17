// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>
#include <vector>

#include "opentelemetry/sdk/logs/logger_context.h"
#include "opentelemetry/sdk/logs/processor.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{

/**
 * Factory class for LoggerContext.
 */
class LoggerContextFactory
{
public:
  /**
   * Create a LoggerContext.
   */
  static std::unique_ptr<LoggerContext> Create(
      std::vector<std::unique_ptr<LogRecordProcessor>> &&processors);

  /**
   * Create a LoggerContext.
   */
  static std::unique_ptr<LoggerContext> Create(
      std::vector<std::unique_ptr<LogRecordProcessor>> &&processors,
      const opentelemetry::sdk::resource::Resource &resource);
};

}  // namespace logs
}  // namespace sdk

OPENTELEMETRY_END_NAMESPACE
