// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/common/key_value_iterable.h"
#include "opentelemetry/common/macros.h"
#include "opentelemetry/metrics/observer_result.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/metrics/state/measurement_attributes_map.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
template <class T>
class ObserverResultT final : public opentelemetry::metrics::ObserverResultT<T>
{
public:
  explicit ObserverResultT() = default;

  ObserverResultT(const ObserverResultT &)            = default;
  ObserverResultT(ObserverResultT &&)                 = default;
  ObserverResultT &operator=(const ObserverResultT &) = default;
  ObserverResultT &operator=(ObserverResultT &&)      = default;

  ~ObserverResultT() override = default;

  void Observe(T value) noexcept override
#if OPENTELEMETRY_HAVE_EXCEPTIONS
  try
#endif
  {
    std::unordered_map<nostd::string_view, opentelemetry::common::AttributeValue> empty;
    data_[empty] += value;
  }
#if OPENTELEMETRY_HAVE_EXCEPTIONS
  catch (...)
  {
    // Silently drop the measurement; per opentelemetry-cpp guidance (PR #3964),
    // exceptions in noexcept API/SDK code must not log or abort.
    return;
  }
#endif

  void Observe(T value, const opentelemetry::common::KeyValueIterable &attributes) noexcept override
#if OPENTELEMETRY_HAVE_EXCEPTIONS
  try
#endif
  {
    std::unordered_map<nostd::string_view, opentelemetry::common::AttributeValue> attr_map;
    attributes.ForEachKeyValue(
        [&](nostd::string_view key, opentelemetry::common::AttributeValue val) noexcept {
          attr_map.emplace(key, val);
          return true;
        });
    data_[attr_map] += value;
  }
#if OPENTELEMETRY_HAVE_EXCEPTIONS
  catch (...)
  {
    // Silently drop the measurement; per opentelemetry-cpp guidance (PR #3964),
    // exceptions in noexcept API/SDK code must not log or abort.
    return;
  }
#endif

  const Measurements<T> &GetMeasurements() { return data_; }

private:
  Measurements<T> data_;
};

}  // namespace metrics
}  // namespace sdk

OPENTELEMETRY_END_NAMESPACE
