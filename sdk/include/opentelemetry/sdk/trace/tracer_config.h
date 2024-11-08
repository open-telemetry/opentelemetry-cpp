// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
/**
 * TracerConfig defines various configurable aspects of a Tracer's behavior.
 */
class TracerConfig {
public:
  /**
   * Returns if the Tracer is enabled or disabled. Tracers are enabled by default.
   * @return a boolean indicating if the Tracer is enabled. Defaults to true.
   */
  bool IsEnabled() const noexcept;

  static TracerConfig Disabled();
  static TracerConfig Enabled();
  static TracerConfig Default();

private:
  explicit TracerConfig(const bool disabled = false) : disabled_(disabled) {}
  bool disabled_;
  static TracerConfig kDefaultConfig;
  static TracerConfig kDisabledConfig;
};
} // namespace trace
} // namespace sdk
OPENTELEMETRY_END_NAMESPACE