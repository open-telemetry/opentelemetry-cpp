// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0/

#pragma once

#include <memory>
#include <vector>

#include "opentelemetry/sdk/logs/logger_context.h"
#include "opentelemetry/sdk/logs/logger_provider.h"
#include "opentelemetry/sdk/logs/processor.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{

/**
 * Factory class for LoggerProvider.
 */
class OPENTELEMETRY_EXPORT LoggerProviderFactory
{
public:

#ifdef OPENTELEMETRY_DEPRECATED_SDK_FACTORY

#ifndef OPENTELEMETRY_NO_DEPRECATED_CODE

  OPENTELEMETRY_DEPRECATED
  static std::unique_ptr<opentelemetry::logs::LoggerProvider> Create(
      std::unique_ptr<LogRecordProcessor> &&processor);

  OPENTELEMETRY_DEPRECATED
  static std::unique_ptr<opentelemetry::logs::LoggerProvider> Create(
      std::unique_ptr<LogRecordProcessor> &&processor,
      const opentelemetry::sdk::resource::Resource &resource);

  OPENTELEMETRY_DEPRECATED
  static std::unique_ptr<opentelemetry::logs::LoggerProvider> Create(
      std::vector<std::unique_ptr<LogRecordProcessor>> &&processors);

  OPENTELEMETRY_DEPRECATED
  static std::unique_ptr<opentelemetry::logs::LoggerProvider> Create(
      std::vector<std::unique_ptr<LogRecordProcessor>> &&processors,
      const opentelemetry::sdk::resource::Resource &resource);

  OPENTELEMETRY_DEPRECATED
  static std::unique_ptr<opentelemetry::logs::LoggerProvider> Create(
      std::unique_ptr<LoggerContext> context);

#endif /* OPENTELEMETRY_NO_DEPRECATED_CODE */

#else

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

#endif /* OPENTELEMETRY_DEPRECATED_SDK_FACTORY */

};

}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
