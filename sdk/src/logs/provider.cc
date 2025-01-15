// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/metrics/provider.h"
#include "opentelemetry/common/spin_lock_mutex.h"
#include "opentelemetry/logs/logger_provider.h"
#include "opentelemetry/logs/provider.h"
#include "opentelemetry/logs/noop.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/sdk/common/disabled.h"
#include "opentelemetry/sdk/logs/provider.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace logs
{

/*
 * Note:
 * This method is declared in the API namespace (for backward compatibility),
 * but part of the SDK.
 */
void Provider::SetLoggerProvider(const nostd::shared_ptr<LoggerProvider> &lp) noexcept
{
  opentelemetry::sdk::logs::Provider::SetLoggerProvider(lp);
}

void Provider::SetEventLoggerProvider(const nostd::shared_ptr<EventLoggerProvider> &lp) noexcept
{
  opentelemetry::sdk::logs::Provider::SetEventLoggerProvider(lp);
}

}  // namespace metrics
OPENTELEMETRY_END_NAMESPACE

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{

void Provider::SetLoggerProvider(
    const nostd::shared_ptr<opentelemetry::logs::LoggerProvider> &lp) noexcept
{
  bool disabled = opentelemetry::sdk::common::GetSdkDisabled();

  if (!disabled)
  {
    std::lock_guard<opentelemetry::common::SpinLockMutex> guard(
        opentelemetry::logs::Provider::GetLock());
    opentelemetry::logs::Provider::GetProvider() = lp;
  }
}

void Provider::SetEventLoggerProvider(
    const nostd::shared_ptr<opentelemetry::logs::EventLoggerProvider> &lp) noexcept
{
  bool disabled = opentelemetry::sdk::common::GetSdkDisabled();

  if (!disabled)
  {
    std::lock_guard<opentelemetry::common::SpinLockMutex> guard(
        opentelemetry::logs::Provider::GetLock());
    opentelemetry::logs::Provider::GetEventProvider() = lp;
  }
}

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
