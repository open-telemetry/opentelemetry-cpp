// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstdint>
#include <memory>
#include <mutex>
#include <vector>

#include "opentelemetry/common/timestamp.h"
#include "opentelemetry/metrics/async_instruments.h"
#include "opentelemetry/metrics/meter.h"
#include "opentelemetry/sdk/metrics/multi_observer_result.h"
#include "opentelemetry/version.h"

#if OPENTELEMETRY_ABI_VERSION_NO >= 2
#  include <unordered_map>
#  include "opentelemetry/nostd/span.h"
#endif

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

struct ObservableCallbackRecord
{
  opentelemetry::metrics::ObservableCallbackPtr callback;
  void *state;
  opentelemetry::metrics::ObservableInstrument *instrument;
};

struct MultiObservableCallbackRecord
{
  opentelemetry::metrics::MultiObservableCallbackPtr callback;
  void *state;
  MultiObserverResult observable_result{};
};

class ObservableRegistry
{
public:
  void AddCallback(opentelemetry::metrics::ObservableCallbackPtr callback,
                   void *state,
                   opentelemetry::metrics::ObservableInstrument *instrument);

  void RemoveCallback(opentelemetry::metrics::ObservableCallbackPtr callback,
                      void *state,
                      opentelemetry::metrics::ObservableInstrument *instrument);
#if OPENTELEMETRY_ABI_VERSION_NO >= 2
  uintptr_t AddMultiCallback(
      opentelemetry::metrics::MultiObservableCallbackPtr callback,
      void *state,
      nostd::span<opentelemetry::metrics::ObservableInstrument *> instruments);
  void RemoveMultiCallback(uintptr_t id);
#endif

  void CleanupCallback(opentelemetry::metrics::ObservableInstrument *instrument);

  void Observe(opentelemetry::common::SystemTimestamp collection_ts);

private:
  std::vector<std::unique_ptr<ObservableCallbackRecord>> callbacks_;
  std::mutex callbacks_m_;
#if OPENTELEMETRY_ABI_VERSION_NO >= 2
  std::unordered_map<uintptr_t, std::unique_ptr<MultiObservableCallbackRecord>> multi_callbacks_;
#endif
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
