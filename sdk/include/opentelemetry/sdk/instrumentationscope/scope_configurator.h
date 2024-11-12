// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <functional>
#include "instrumentation_scope.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace instrumentationscope
{
template <typename T>
class ScopeConfigurator
{
public:
  static nostd::unique_ptr<ScopeConfigurator> Create(
      std::function<T(const InstrumentationScope &)> scope_configurator)
  {
    return nostd::unique_ptr<ScopeConfigurator>(new ScopeConfigurator(scope_configurator));
  }

  T ComputeConfig(const InstrumentationScope &instrumentation_scope)
  {
    return scope_configurator_(instrumentation_scope);
  }

private:
  explicit ScopeConfigurator(
      const std::function<T(const InstrumentationScope &)> scope_configurator)
      : scope_configurator_(scope_configurator)
  {}
  const std::function<T(const InstrumentationScope &)> scope_configurator_;
};
}  // namespace instrumentationscope
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
