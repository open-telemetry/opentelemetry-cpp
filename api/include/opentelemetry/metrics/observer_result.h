// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW

#  include "opentelemetry/common/attribute_value.h"
#  include "opentelemetry/common/key_value_iterable_view.h"
#  include "opentelemetry/nostd/span.h"
#  include "opentelemetry/nostd/string_view.h"
#  include "opentelemetry/nostd/type_traits.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace metrics
{

/**
 * ObserverResult class is necessary for the callback recording asynchronous
 * instrument use.
 */

template <class T>
class ObserverResult
{

public:
  virtual void Observe(T value) noexcept = 0;

  virtual void Observe(T value, const common::KeyValueIterable &attributes) noexcept = 0;

  template <class U,
            nostd::enable_if_t<common::detail::is_key_value_iterable<U>::value> * = nullptr>
  void Observe(T value, const U &attributes) noexcept
  {
    this->Observe(value, common::KeyValueIterableView<U>{attributes});
  }

  void Observe(T value,
               std::initializer_list<std::pair<nostd::string_view, common::AttributeValue>>
                   attributes) noexcept
  {
    this->Observe(value, nostd::span<const std::pair<nostd::string_view, common::AttributeValue>>{
                             attributes.begin(), attributes.end()});
  }
};

}  // namespace metrics
OPENTELEMETRY_END_NAMESPACE
#endif
