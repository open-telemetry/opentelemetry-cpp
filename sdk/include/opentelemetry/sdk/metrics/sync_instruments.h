// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW

#  include "opentelemetry/metrics/sync_instruments.h"
#  include "opentelemetry/sdk/metrics/instruments.h"
#  include "opentelemetry/sdk/metrics/state/writable_metrics_storage.h"
#  include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

template <class T>
class Counter : public openetelemetry::metrics::Counter<T>
{
public:
  Counter(std::unique_ptr<InstrumentDescriptor> &&instrument_descriptor,
          std::unique_ptr<WritableMetricsStorage<T>> &&writable_storage)
      : instrument_descriptor_{std::move(instrument_descriptor)},
        writable_storage_{std::move(writable_storage)}
  {}
  void Add(T value) noexcept override
  {
    if (value < 0)
    {
      // TBD -warning
      return;
    }
    writable_storage_->Record(value);
  }
  void Add(T value, const common::KeyValueIterable &attributes) noexcept override
  {
    if (value < 0)
    {
      // TBD -warning
      return;
    }
    writable_storage_->Record(value, attributes);
  }

private:
  std::unique_ptr<InstrumentDescriptor> instrument_descriptor_;
  std::unique_ptr<WritableMetricsStorage<T>> writable_storage_;
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
