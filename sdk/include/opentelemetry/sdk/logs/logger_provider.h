// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0/

#pragma once
#ifdef ENABLE_LOGS_PREVIEW

#  include <memory>
#  include <mutex>
#  include <string>
#  include <vector>

#  include "opentelemetry/logs/logger_provider.h"
#  include "opentelemetry/logs/noop.h"
#  include "opentelemetry/nostd/shared_ptr.h"
#  include "opentelemetry/sdk/common/atomic_shared_ptr.h"
#  include "opentelemetry/sdk/logs/logger.h"
#  include "opentelemetry/sdk/logs/logger_context.h"
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

class LoggerProvider final : public opentelemetry::logs::LoggerProvider
{
public:
  /**
   * Initialize a new logger provider
   * @param processor The span processor for this logger provider. This must
   * not be a nullptr.
   * @param resource  The resources for this logger provider.
   * @param sampler The sampler for this logger provider. This must
   * not be a nullptr.
   * @param id_generator The custom id generator for this logger provider. This must
   * not be a nullptr
   */
  explicit LoggerProvider(std::unique_ptr<LogProcessor> &&processor,
                          opentelemetry::sdk::resource::Resource resource =
                              opentelemetry::sdk::resource::Resource::Create({})) noexcept;

  explicit LoggerProvider(std::vector<std::unique_ptr<LogProcessor>> &&processors,
                          opentelemetry::sdk::resource::Resource resource =
                              opentelemetry::sdk::resource::Resource::Create({})) noexcept;

  /**
   * Initialize a new logger provider. A processor must later be assigned
   * to this logger provider via the AddProcessor() method.
   */
  explicit LoggerProvider() noexcept;

  /**
   * Initialize a new logger provider with a specified context
   * @param context The shared logger configuration/pipeline for this provider.
   */
  explicit LoggerProvider(std::shared_ptr<sdk::logs::LoggerContext> context) noexcept;

  ~LoggerProvider();

  /**
   * Creates a logger with the given name, and returns a shared pointer to it.
   * If a logger with that name already exists, return a shared pointer to it
   * @param logger_name The name of the logger to be created.
   * @param options The options for the logger. TODO: Once the logging spec defines it,
   * give a list of options that the logger supports.
   * @param library_name The version of the library.
   * @param library_version The version of the library.
   * @param schema_url The schema URL.
   */
  nostd::shared_ptr<opentelemetry::logs::Logger> GetLogger(
      nostd::string_view logger_name,
      nostd::string_view options,
      nostd::string_view library_name,
      nostd::string_view library_version = "",
      nostd::string_view schema_url      = "") noexcept override;
  /**
   * Creates a logger with the given name, and returns a shared pointer to it.
   * If a logger with that name already exists, return a shared pointer to it
   * @param name The name of the logger to be created.
   * @param args The arguments for the logger. TODO: Once the logging spec defines it,
   * give a list of arguments that the logger supports.
   * @param library_name The version of the library.
   * @param library_version The version of the library.
   * @param schema_url The schema URL.
   */
  nostd::shared_ptr<opentelemetry::logs::Logger> GetLogger(
      nostd::string_view logger_name,
      nostd::span<nostd::string_view> args,
      nostd::string_view library_name,
      nostd::string_view library_version = "",
      nostd::string_view schema_url      = "") noexcept override;

  /**
   * Add the processor that is stored internally in the logger provider.
   * @param processor The processor to be stored inside the logger provider.
   * This must not be a nullptr.
   */
  void AddProcessor(std::unique_ptr<LogProcessor> processor) noexcept;

  /**
   * Obtain the resource associated with this logger provider.
   * @return The resource for this logger provider.
   */
  const opentelemetry::sdk::resource::Resource &GetResource() const noexcept;

  /**
   * Shutdown the log processor associated with this log provider.
   */
  bool Shutdown() noexcept;

  /**
   * Force flush the log processor associated with this log provider.
   */
  bool ForceFlush(std::chrono::microseconds timeout = (std::chrono::microseconds::max)()) noexcept;

private:
  // order of declaration is important here - loggers should destroy only after context.
  std::vector<std::shared_ptr<opentelemetry::sdk::logs::Logger>> loggers_;
  std::shared_ptr<sdk::logs::LoggerContext> context_;
  std::mutex lock_;
};
}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
