// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include <unordered_map>
#  include "opentelemetry/sdk/instrumentationscope/instrumentation_scope.h"
#  include "opentelemetry/sdk/metrics/view/instrument_selector.h"
#  include "opentelemetry/sdk/metrics/view/meter_selector.h"
#  include "opentelemetry/sdk/metrics/view/view.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
struct RegisteredView
{
  RegisteredView(
      std::unique_ptr<opentelemetry::sdk::metrics::InstrumentSelector> instrument_selector,
      std::unique_ptr<opentelemetry::sdk::metrics::MeterSelector> meter_selector,
      std::unique_ptr<opentelemetry::sdk::metrics::View> view)
      : instrument_selector_{std::move(instrument_selector)},
        meter_selector_{std::move(meter_selector)},
        view_{std::move(view)}
  {}
  std::unique_ptr<opentelemetry::sdk::metrics::InstrumentSelector> instrument_selector_;
  std::unique_ptr<opentelemetry::sdk::metrics::MeterSelector> meter_selector_;
  std::unique_ptr<opentelemetry::sdk::metrics::View> view_;
};

class ViewRegistry
{
public:
  void AddView(std::unique_ptr<opentelemetry::sdk::metrics::InstrumentSelector> instrument_selector,
               std::unique_ptr<opentelemetry::sdk::metrics::MeterSelector> meter_selector,
               std::unique_ptr<opentelemetry::sdk::metrics::View> view)
  {
    // TBD - thread-safe ?

    auto registered_view = std::unique_ptr<RegisteredView>(new RegisteredView{
        std::move(instrument_selector), std::move(meter_selector), std::move(view)});
    registered_views_.push_back(std::move(registered_view));
  }

  bool FindViews(
      const opentelemetry::sdk::metrics::InstrumentDescriptor &instrument_descriptor,
      const opentelemetry::sdk::instrumentationscope::InstrumentationScope &instrumentation_scope,
      nostd::function_ref<bool(const View &)> callback)
  {
    bool found = findViewsInternal(instrument_descriptor, instrumentation_scope, callback);
    // create default view if none found;
    if (!found)
    {
      createView(instrument_descriptor, instrumentation_scope);
      return findViewsInternal(instrument_descriptor, instrumentation_scope, callback);
    }
    return true;
  }

  ~ViewRegistry() = default;

private:
  std::vector<std::unique_ptr<RegisteredView>> registered_views_;
  static bool MatchMeter(
      opentelemetry::sdk::metrics::MeterSelector *selector,
      const opentelemetry::sdk::instrumentationscope::InstrumentationScope &instrumentation_scope)
  {
    return selector->GetNameFilter()->Match(instrumentation_scope.GetName()) &&
           (instrumentation_scope.GetVersion().size() == 0 ||
            selector->GetVersionFilter()->Match(instrumentation_scope.GetVersion())) &&
           (instrumentation_scope.GetSchemaURL().size() == 0 ||
            selector->GetSchemaFilter()->Match(instrumentation_scope.GetSchemaURL()));
  }

  static bool MatchInstrument(
      opentelemetry::sdk::metrics::InstrumentSelector *selector,
      const opentelemetry::sdk::metrics::InstrumentDescriptor &instrument_descriptor)
  {
    return selector->GetNameFilter()->Match(instrument_descriptor.name_) &&
           (selector->GetInstrumentType() == instrument_descriptor.type_);
  }

  bool findViewsInternal(
      const opentelemetry::sdk::metrics::InstrumentDescriptor &instrument_descriptor,
      const opentelemetry::sdk::instrumentationscope::InstrumentationScope &instrumentation_scope,
      nostd::function_ref<bool(const View &)> callback) const
  {
    bool found = false;
    for (auto const &registered_view : registered_views_)
    {
      if (MatchMeter(registered_view->meter_selector_.get(), instrumentation_scope) &&
          MatchInstrument(registered_view->instrument_selector_.get(), instrument_descriptor))
      {
        found = true;
        if (!callback(*(registered_view->view_.get())))
        {
          return false;
        }
      }
    }
    return found;
  }

  void createView(
      const opentelemetry::sdk::metrics::InstrumentDescriptor &instrument_descriptor,
      const opentelemetry::sdk::instrumentationscope::InstrumentationScope &instrumentation_scope)
  {
    AddView(
        std::unique_ptr<opentelemetry::sdk::metrics::InstrumentSelector>{
            new opentelemetry::sdk::metrics::InstrumentSelector{instrument_descriptor.type_,
                                                                instrument_descriptor.name_}},
        std::unique_ptr<opentelemetry::sdk::metrics::MeterSelector>{
            new opentelemetry::sdk::metrics::MeterSelector{instrumentation_scope.GetName(),
                                                           instrumentation_scope.GetVersion(),
                                                           instrumentation_scope.GetSchemaURL()}},
        std::unique_ptr<opentelemetry::sdk::metrics::View>{
            new opentelemetry::sdk::metrics::View{instrumentation_scope.GetName()}});
  }
};
};  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
