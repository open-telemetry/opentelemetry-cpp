// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0/

#pragma once
#ifdef ENABLE_LOGS_PREVIEW

#  include <memory>
#  include <mutex>
#  include <string>
#  include <unordered_map>
#  include <vector>

#  include "opentelemetry/logs/logger_provider.h"
#  include "opentelemetry/logs/noop.h"
#  include "opentelemetry/nostd/shared_ptr.h"
#  include "opentelemetry/sdk/common/atomic_shared_ptr.h"
#  include "opentelemetry/sdk/logs/logger.h"
#  include "opentelemetry/sdk/logs/processor.h"

// Define the maximum number of loggers that are allowed to be registered to the loggerprovider.
// TODO: Add link to logging spec once this is added to it
#  define MAX_LOGGER_COUNT 100

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{
class Logger;

class LoggerProvider final : public opentelemetry::logs::LoggerProvider,
                             public std::enable_shared_from_this<LoggerProvider>
{
public:
  /**
   * Initialize a new logger provider. A processor must later be assigned
   * to this logger provider via the SetProcessor() method.
   */
  explicit LoggerProvider() noexcept;

  /**
   * Creates a logger with the given name, and returns a shared pointer to it.
   * If a logger with that name already exists, return a shared pointer to it
   * @param name The name of the logger to be created.
   * @param options (OPTIONAL) The options for the logger. TODO: Once the logging spec defines it,
   * give a list of options that the logger supports.
   */
  opentelemetry::nostd::shared_ptr<opentelemetry::logs::Logger> GetLogger(
      opentelemetry::nostd::string_view name,
      opentelemetry::nostd::string_view options = "") noexcept override;

  /**
   * Creates a logger with the given name, and returns a shared pointer to it.
   * If a logger with that name already exists, return a shared pointer to it
   * @param name The name of the logger to be created.
   * @param args (OPTIONAL) The arguments for the logger. TODO: Once the logging spec defines it,
   * give a list of arguments that the logger supports.
   */
  opentelemetry::nostd::shared_ptr<opentelemetry::logs::Logger> GetLogger(
      opentelemetry::nostd::string_view name,
      nostd::span<nostd::string_view> args) noexcept override;

  /**
   * Returns a shared pointer to the processor currently stored in the
   * logger provider. If no processor exists, returns a nullptr
   */
  std::shared_ptr<LogProcessor> GetProcessor() noexcept;

  // Sets the common processor that all the Logger instances will use
  /**
   * Sets the processor that is stored internally in the logger provider.
   * @param processor The processor to be stored inside the logger provider.
   * This must not be a nullptr.
   */
  void SetProcessor(std::shared_ptr<LogProcessor> processor) noexcept;

private:
  // A pointer to the processor stored by this logger provider
  opentelemetry::sdk::common::AtomicSharedPtr<LogProcessor> processor_;

  // A vector of pointers to all the loggers that have been created
  std::unordered_map<std::string, opentelemetry::nostd::shared_ptr<opentelemetry::logs::Logger>>
      loggers_;

  // A mutex that ensures only one thread is using the map of loggers
  std::mutex mu_;
};
}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
