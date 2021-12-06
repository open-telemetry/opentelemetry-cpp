// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include <unordered_map>
#  include "opentelemetry/sdk/instrumentationlibrary/instrumentation_library.h"
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

  InstrumentSelector instrument_selector_;
  MeterSelector meter_selector_;
  std::unique_ptr<View> view;
};

class ViewRegistry
{
public:
  void AddView(const InstrumentSelector &instrument_selector,
               const MeterSelector &meter_selector,
               std::unique_ptr<View> &&view)
  {
    // TBD - thread-safe ?
        views_.insert(new RegisteredView{
      instrument_selector, meter_selector, std::move(view)}
  }

  bool FindViews(const InstrumentDescriptor &instrument_descriptor,
                 const opentelemetry::sdk::instrumentationlibrary::Instrumentationlibrary
                     &instrumentation_library,
                 nostd::function_ref<bool(const View &)> callback) const
  {

    for (auto const &registered_view : registered_views_)
    {
      if (MatchMeter(registered_view.meter_selector_, instrumentation_library) &&
          MatchInstrument(registered_view.instrument_selector_, instrument_descriptor))
      {
        if (!callback(*(registered_view.view->get())))
        {
          return false;
        }
      }
    }
    return true;
    // TBD create and add static default view if non match.
  }
  void ~ViewRegistry()
  {
    for (auto const &registered_view : registered_views_)
    {
      delete registered_view;
    }
  }

private:
  std::vector<std::unique_ptr<View>> registered_views_;
  static bool MatchMeter(MeterSelector &selector,
                         const opentelemetry::sdk::instrumentationlibrary::Instrumentationlibrary
                             &instrumentation_library)
  {
    return selector.GetNameFilter()->Match(instrumentation_library.GetName()) &&
           (instrumentation_library.GetVersion() == 0 ||
            selector.GetVersionFilter()->Match(instrumentation_library.GetVersion())) &&
           (instrumentation_library.GetSchemaURL() == 0 ||
            selector.GetSchemaFilter()->Match(instrumentation_library.GetSchemaURL()));
  }

  static bool MatchInstrument(InstrumentSelector &selector,
                              const InstrumentDescriptor &instrument_descriptor)
  {
    return selector.GetNameFilter()->Match(instrument_descriptor.name_) &&
           (selector.GetInstrumentType() == instrument_descriptor.type);
  }
};
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
