// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/metrics/provider.h"
#include "opentelemetry/common/spin_lock_mutex.h"
#include "opentelemetry/logs/logger_provider.h"
#include "opentelemetry/logs/noop.h"
#include "opentelemetry/logs/provider.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/sdk/common/disabled.h"
#include "opentelemetry/sdk/logs/provider.h"
#include "opentelemetry/version.h"

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
    opentelemetry::logs::Provider::SetLoggerProvider(lp);
  }
}

void Provider::SetEventLoggerProvider(
    const nostd::shared_ptr<opentelemetry::logs::EventLoggerProvider> &lp) noexcept
{
  bool disabled = opentelemetry::sdk::common::GetSdkDisabled();

  if (!disabled)
  {
    opentelemetry::logs::Provider::SetEventLoggerProvider(lp);
  }
}

}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE