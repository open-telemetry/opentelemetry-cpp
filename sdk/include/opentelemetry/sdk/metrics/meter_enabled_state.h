// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/common/macros.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

/**
 * A Meter's enabled flag, shared with every instrument it creates so a configurator update
 * reaches instruments that already exist. Relaxed ordering: no dependent data, only visibility.
 * A plain bool, not std::atomic, because API instruments read it through Flag().
 */
class MeterEnabledState
{
public:
  explicit MeterEnabledState(bool enabled = true) noexcept { flag_.enabled = enabled; }

  ~MeterEnabledState()                                    = default;
  MeterEnabledState(const MeterEnabledState &)            = delete;
  MeterEnabledState(MeterEnabledState &&)                 = delete;
  MeterEnabledState &operator=(const MeterEnabledState &) = delete;
  MeterEnabledState &operator=(MeterEnabledState &&)      = delete;

  bool IsEnabled() const noexcept
  {
    return OPENTELEMETRY_ATOMIC_READ_RELAXED_8(&flag_.enabled) != 0;
  }

  void SetEnabled(bool enabled) noexcept { OPENTELEMETRY_ATOMIC_WRITE_8(&flag_.enabled, enabled); }

  /** The flag itself, valid for the lifetime of this object. */
  const bool *Flag() const noexcept { return &flag_.enabled; }

private:
  // Padding keeps the flag off cache lines that neighbouring heap objects write to, since every
  // instrument of the meter reads it on each Enabled() call.
  struct PaddedFlag
  {
    char padding_before[64];
    bool enabled;
    char padding_after[63];
  };
  PaddedFlag flag_{};
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
