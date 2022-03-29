// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/common/key_value_iterable_view.h"
#  include "opentelemetry/sdk/metrics/instruments.h"
#  include "opentelemetry/sdk/metrics/state/metric_storage.h"

#  include <memory>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

class MultiMetricStorage : public WritableMetricStorage
{
public:
  void AddStorage(std::shared_ptr<WritableMetricStorage> storage) { storages_.push_back(storage); }

  virtual void RecordLong(long value,
                          const opentelemetry::context::Context &context) noexcept override
  {
    for (auto &s : storages_)
    {
      s->RecordLong(value, context);
    }
  }

  virtual void RecordLong(long value,
                          const opentelemetry::common::KeyValueIterable &attributes,
                          const opentelemetry::context::Context &context) noexcept override
  {
    for (auto &s : storages_)
    {
      s->RecordLong(value, attributes, context);
    }
  }

  virtual void RecordDouble(double value,
                            const opentelemetry::context::Context &context) noexcept override
  {
    for (auto &s : storages_)
    {
      s->RecordDouble(value, context);
    }
  }

  virtual void RecordDouble(double value,
                            const opentelemetry::common::KeyValueIterable &attributes,
                            const opentelemetry::context::Context &context) noexcept override
  {
    for (auto &s : storages_)
    {
      s->RecordDouble(value, attributes, context);
    }
  }

private:
  std::vector<std::shared_ptr<WritableMetricStorage>> storages_;
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
