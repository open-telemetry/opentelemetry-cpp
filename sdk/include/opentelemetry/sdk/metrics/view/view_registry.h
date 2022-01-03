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

  bool FindViews(const opentelemetry::sdk::metrics::InstrumentDescriptor &instrument_descriptor,
                 const opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary
                     &instrumentation_library,
                 nostd::function_ref<bool(const View &)> callback) const
  {
    bool found = false;
    for (auto const &registered_view : registered_views_)
    {
      if (MatchMeter(registered_view->meter_selector_.get(), instrumentation_library) &&
          MatchInstrument(registered_view->instrument_selector_.get(), instrument_descriptor))
      {
        found = true;
        if (!callback(*(registered_view->view_.get())))
        {
          return false;
        }
      }
    }
    // return default view if none found;
    if (!found)
    {
      static View view("");
      if (!callback(view))
      {
        return false;
      }
    }
    return true;
  }

  ~ViewRegistry() = default;

private:
  std::vector<std::unique_ptr<RegisteredView>> registered_views_;
  static bool MatchMeter(opentelemetry::sdk::metrics::MeterSelector *selector,
                         const opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary
                             &instrumentation_library)
  {
    return selector->GetNameFilter()->Match(instrumentation_library.GetName()) &&
           (instrumentation_library.GetVersion().size() == 0 ||
            selector->GetVersionFilter()->Match(instrumentation_library.GetVersion())) &&
           (instrumentation_library.GetSchemaURL().size() == 0 ||
            selector->GetSchemaFilter()->Match(instrumentation_library.GetSchemaURL()));
  }

  static bool MatchInstrument(
      opentelemetry::sdk::metrics::InstrumentSelector *selector,
      const opentelemetry::sdk::metrics::InstrumentDescriptor &instrument_descriptor)
  {
    return selector->GetNameFilter()->Match(instrument_descriptor.name_) &&
           (selector->GetInstrumentType() == instrument_descriptor.type_);
  }
};
};  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
