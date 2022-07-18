// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW

#  include "opentelemetry/common/timestamp.h"
#  include "opentelemetry/sdk/metrics/async_instruments.h"
#  include "opentelemetry/sdk/metrics/observer_result.h"

#  include <memory>
#  include <mutex>
#  include <unordered_set>
#  include <vector>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

struct ObservableCallbackRecord
{
  opentelemetry::metrics::ObservableCallbackPtr callback;
  void *state;
  ObservableInstrument *instrument;
};

class ObservableRegistry
{
public:
  void AddCallback(opentelemetry::metrics::ObservableCallbackPtr callback,
                   void *state,
                   ObservableInstrument *instrument)
  {
    // TBD - Check if existing
    std::unique_ptr<ObservableCallbackRecord> record(
        new ObservableCallbackRecord{callback, state, instrument});
    std::unique_lock<std::mutex> lk(callbacks_m_);
    callbacks_.push_back(std::move(record));
  }

  void RemoveCallback(opentelemetry::metrics::ObservableCallbackPtr callback,
                      void *state,
                      ObservableInstrument *instrument)
  {
    auto new_end = std::remove_if(
        callbacks_.begin(), callbacks_.end(),
        [callback, state, instrument](const std::unique_ptr<ObservableCallbackRecord> &record) {
          return record->callback == callback && record->state == state &&
                 record->instrument == instrument;
        });
    std::unique_lock<std::mutex> lk(callbacks_m_);
    callbacks_.erase(new_end, callbacks_.end());
  }

  void Observe(opentelemetry::common::SystemTimestamp collection_ts)
  {
    std::unique_lock<std::mutex> lk(callbacks_m_);
    for (auto &callback_wrap : callbacks_)
    {
      if (callback_wrap->instrument->GetInstrumentDescriptor().value_type_ ==
          InstrumentValueType::kDouble)
      {
        nostd::shared_ptr<opentelemetry::metrics::ObserverResultT<double>> ob_res(
            new opentelemetry::sdk::metrics::ObserverResultT<double>());
        callback_wrap->callback(ob_res, callback_wrap->state);
      }
      else
      {
        nostd::shared_ptr<opentelemetry::metrics::ObserverResultT<long>> ob_res(
            new opentelemetry::sdk::metrics::ObserverResultT<long>());
        callback_wrap->callback(ob_res, callback_wrap->state);
      }
    }
  }

private:
  std::vector<std::unique_ptr<ObservableCallbackRecord>> callbacks_;
  std::mutex callbacks_m_;
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif