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
                                              nostd::string_view options,
                                              nostd::string_view library_name,
                                              nostd::string_view library_version = "",
                                              nostd::string_view schema_url      = "") = 0;

  virtual nostd::shared_ptr<Logger> GetLogger(nostd::string_view logger_name,
                                              nostd::span<nostd::string_view> args,
                                              nostd::string_view library_name,
                                              nostd::string_view library_version = "",
                                              nostd::string_view schema_url      = "") = 0;
};
}  // namespace logs
OPENTELEMETRY_END_NAMESPACE

#  if defined(OPENTELEMETRY_EXPORT)

namespace std
{

//
// Partial specialization of default_delete used by unique_ptr or shared_ptr.
// This makes the delete of the type in unique_ptr/shared_ptr happening in the
// DLL in which it is allocated.
//
template <>
class OPENTELEMETRY_EXPORT default_delete<OPENTELEMETRY_NAMESPACE::logs::LoggerProvider>
{
public:
  void operator()(OPENTELEMETRY_NAMESPACE::logs::LoggerProvider *logger_provider)
  {
    delete logger_provider;
  }
};

}  // namespace std

#  endif  // defined(OPENTELEMETRY_EXPORT)

#endif  // ENABLE_LOGS_PREVIEW
