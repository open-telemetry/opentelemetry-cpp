// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <atomic>
#include <iostream>
#include <mutex>

#include "opentelemetry/common/spin_lock_mutex.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/version.h"

#ifndef IGNORE_OTEL_ERROR_LOGGER
#  define OTEL_ERROR(message) \
    opentelemetry::sdk::common::Error::GlobalErrorHandler::GetErrorHandler()->Handle(message);
#else
#  define OTEL_ERROR(message)
#endif

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace common
{
namespace Error
{

class ErrorHandler
{
public:
  virtual void Handle(nostd::string_view error) noexcept = 0;
};

class DefaultErrorHandler : public ErrorHandler
{
public:
  void Handle(nostd::string_view error) noexcept override
  {
    // std::cout::operator<<() should be thread safe without any chaining calls.
    std::cerr << error;
  }
};

class NoopErrorHandler : public ErrorHandler
{
public:
  void Handle(nostd::string_view error) noexcept override
  {
    // ignore the error message
  }
};

/**
 * Stores the singleton global ErrorHandler.
 */
class GlobalErrorHandler
{
public:
  /**
   * Returns the singleton ErrorHandler.
   *
   * By default, a default ErrorHandler is returned. This will never return a
   * nullptr ErrorHandler.
   */
  static nostd::shared_ptr<ErrorHandler> GetErrorHandler() noexcept
  {
    std::lock_guard<opentelemetry::common::SpinLockMutex> guard(GetLock());
    return nostd::shared_ptr<ErrorHandler>(GetHandler());
  }

  /**
   * Changes the singleton ErrorHandler.
   */
  static void SetErrorHandler(nostd::shared_ptr<ErrorHandler> eh) noexcept
  {
    std::lock_guard<opentelemetry::common::SpinLockMutex> guard(GetLock());
    GetHandler() = eh;
  }

private:
  static nostd::shared_ptr<ErrorHandler> &GetHandler() noexcept
  {
    static nostd::shared_ptr<ErrorHandler> handler(new DefaultErrorHandler);
    return handler;
  }

  static opentelemetry::common::SpinLockMutex &GetLock() noexcept
  {
    static opentelemetry::common::SpinLockMutex lock;
    return lock;
  }
};

}  // namespace Error
}  // namespace common
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE