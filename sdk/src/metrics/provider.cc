// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/metrics/provider.h"
#include "opentelemetry/common/spin_lock_mutex.h"
#include "opentelemetry/metrics/meter_provider.h"
#include "opentelemetry/metrics/noop.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/sdk/common/disabled.h"
#include "opentelemetry/sdk/metrics/provider.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace metrics
{

/*
 * Note:
 * This method is declared in the API namespace (for backward compatibility),
 * but part of the SDK.
 */
void Provider::SetMeterProvider(const nostd::shared_ptr<MeterProvider> &mp) noexcept
{
  opentelemetry::sdk::metrics::Provider::SetMeterProvider(mp);
}

}  // namespace metrics
OPENTELEMETRY_END_NAMESPACE

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

void Provider::SetMeterProvider(
    const nostd::shared_ptr<opentelemetry::metrics::MeterProvider> &mp) noexcept
{
  bool disabled = opentelemetry::sdk::common::GetSdkDisabled();

  if (!disabled)
  {
    std::lock_guard<opentelemetry::common::SpinLockMutex> guard(
        opentelemetry::metrics::Provider::GetLock());
    opentelemetry::metrics::Provider::GetProvider() = mp;
  }
}

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
