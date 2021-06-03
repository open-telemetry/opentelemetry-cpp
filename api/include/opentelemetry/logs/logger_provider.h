// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifdef ENABLE_LOGS_PREVIEW

#  include "opentelemetry/logs/logger.h"
#  include "opentelemetry/nostd/shared_ptr.h"
#  include "opentelemetry/nostd/string_view.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace logs
{
/**
 * Creates new Logger instances.
 */
class LoggerProvider
{
public:
  virtual ~LoggerProvider() = default;

  /**
   * Gets or creates a named Logger instance.
   *
   * Optionally a version can be passed to create a named and versioned Logger
   * instance.
   *
   * Optionally a configuration file name can be passed to create a configuration for
   * the Logger instance.
   *
   */

  virtual nostd::shared_ptr<Logger> GetLogger(nostd::string_view logger_name,
                                              nostd::string_view options = "") = 0;

  virtual nostd::shared_ptr<Logger> GetLogger(nostd::string_view logger_name,
                                              nostd::span<nostd::string_view> args) = 0;
};
}  // namespace logs
OPENTELEMETRY_END_NAMESPACE
#endif
