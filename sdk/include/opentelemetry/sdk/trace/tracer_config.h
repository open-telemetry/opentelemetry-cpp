// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <functional>
#include "opentelemetry/sdk/instrumentationscope/instrumentation_scope.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
// Forward declaration to support typedef creation
class TracerConfig;

typedef std::function<TracerConfig(const instrumentationscope::InstrumentationScope &)>
    TracerConfigurator;
/**
 * TracerConfig defines various configurable aspects of a Tracer's behavior.
 */
class TracerConfig
{
public:
  /**
   * Returns if the Tracer is enabled or disabled. Tracers are enabled by default.
   * @return a boolean indicating if the Tracer is enabled. Defaults to true.
   */
  bool IsEnabled() const noexcept;
  bool operator==(const TracerConfig &other) const noexcept;

  static TracerConfig Disabled();
  static TracerConfig Enabled();
  static TracerConfig Default();
  static const TracerConfigurator &DefaultConfigurator();

private:
  explicit TracerConfig(const bool disabled = false) : disabled_(disabled) {}
  bool disabled_;
  static const TracerConfig kDefaultConfig;
  static const TracerConfig kDisabledConfig;
  static const TracerConfigurator kDefaultTracerConfigurator;
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
