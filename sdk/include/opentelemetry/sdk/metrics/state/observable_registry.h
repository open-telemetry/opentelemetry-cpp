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
  ObservableInstrument *instrument;
};

class ObservableRegistry
{
public:
  void AddCallback(ObservableCallbackPtr callback, ObservableInstrument *instrument)
  {
    // TBD - Check if existing
    auto record =
        std::unique_ptr<ObservableCallbackRecord>(ObservableCallbackRecord{callback, instrument});
    callbacks_.push_back(std::move(record));
  }

  void RemoveCallback(ObservableCallbackPtr callback, ObservableInstrument *instrument)
  {
    auto new_end = std::remove_if(
        callbacks_.begin(), callbacks_.end(),
        [callback, instrument](const std::unique_ptr<ObservableCallbackRecord> &record) {
          return record->callback == callback && record->instrument == instrument;
        });
    callbacks_.erase(new_end, callbacks_.end());
  }

private:
  std::vector<std::unique_ptr<ObservableCallbackRecord>> callbacks_;
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif