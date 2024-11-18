// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/sdk/instrumentationscope/instrumentation_scope.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
/**
 * TracerConfig defines various configurable aspects of a Tracer's behavior.
 * This class should not be used directly to configure a Tracer's behavior, instead a
 * ScopeConfigurator should be used to compute the desired TracerConfig which can then be used to
 * configure a Tracer.
 */
class TracerConfig
{
public:
  bool operator==(const TracerConfig &other) const noexcept;

  /**
   * Returns if the Tracer is enabled or disabled. Tracers are enabled by default.
   * @return a boolean indicating if the Tracer is enabled. Defaults to true.
   */
  bool IsEnabled() const noexcept;

  /**
   * Returns a TracerConfig that represents a disabled Tracer. A disabled tracer behaves like a
   * no-op tracer.
   * @return a static constant TracerConfig that represents a disabled tracer.
   */
  static TracerConfig Disabled();

  /**
   * Returns a TracerConfig that represents an enabled Tracer.
   * @return a static constant TracerConfig that represents an enabled tracer.
   */
  static TracerConfig Enabled();

  /**
   * Returns a TracerConfig that represents a Tracer configured with the default behavior.
   * The default behavior is guided by the OpenTelemetry specification.
   * @return a static constant TracerConfig that represents a tracer configured with default
   * behavior.
   */
  static TracerConfig Default();

  /**
   * Returns a ScopeConfigurator that can be used to get the default tracer configurator.
   * A tracer configurator computes TracerConfig based on the InstrumentationScope. The default
   * tracer configurator unconditionally computes the default TracerConfig for all scopes.
   *
   * @return a static constant TracerConfig that represents a tracer configurator with default
   * behavior.
   */
  static const instrumentationscope::ScopeConfigurator<TracerConfig> &DefaultConfigurator();

private:
  explicit TracerConfig(const bool disabled = false) : disabled_(disabled) {}
  bool disabled_;
  static const TracerConfig kDefaultConfig;
  static const TracerConfig kDisabledConfig;
  static const instrumentationscope::ScopeConfigurator<TracerConfig> kDefaultTracerConfigurator;
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
