// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW

#  include "opentelemetry/sdk/metrics/async_instruments.h"

#  include <memory>
#  include <unordered_set>
#  include <vector>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

struct ObservableCallbackRecord
{
  ObservableCallbackPtr callback;
  void *state;
  ObservableInstrument *instrument;
};

class ObservableRegistry
{
public:
  void AddCallback(ObservableCallbackPtr callback, void *state, ObservableInstrument *instrument)
  {
    // TBD - Check if existing
    auto record = std::unique_ptr<ObservableCallbackRecord>(
        ObservableCallbackRecord{callback, state, instrument});
    std::unique_lock<std::mutex> lk(callbacks_m_);
    callbacks_.push_back(std::move(record));
  }

  void RemoveCallback(ObservableCallbackPtr callback, void *state, ObservableInstrument *instrument)
  {
    auto new_end = std::remove_if(
        callbacks_.begin(), callbacks_.end(),
        [callback, instrument](const std::unique_ptr<ObservableCallbackRecord> &record) {
          return record->callback == callback && record->state == state &&
                 record->instrument == instrument;
        });
    std::unique_lock<std::mutex> lk(callbacks_m_);
    callbacks_.erase(new_end, callbacks_.end());
  }

  void Observe(opentelemetry::common::SystemTimestamp collection_ts)
  {
    std::unique_lock<std::mutex> lk(callbacks_m_);
    for (auto callback : callbacks_)
    {
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