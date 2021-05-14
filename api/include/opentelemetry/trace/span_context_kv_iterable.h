// Copyright 2021, OpenTelemetry Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/common/key_value_iterable_view.h"
#include "opentelemetry/nostd/function_ref.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace trace
{
/**
 * Supports internal iteration over a collection of SpanContext/key-value pairs.
 */
class SpanContextKeyValueIterable
{
public:
  virtual ~SpanContextKeyValueIterable() = default;

  /**
   * Iterate over SpanContext/key-value pairs
   * @param callback a callback to invoke for each key-value for each SpanContext.
   * If the callback returns false, the iteration is aborted.
   * @return true if every SpanContext/key-value pair was iterated over
   */
  virtual bool ForEachKeyValue(
      nostd::function_ref<bool(SpanContext, const opentelemetry::common::KeyValueIterable &)>
          callback) const noexcept = 0;
  /**
   * @return the number of key-value pairs
   */
  virtual size_t size() const noexcept = 0;
};

/**
 * @brief Null Span context that does not carry any information.
 */
class NullSpanContext : public SpanContextKeyValueIterable
{
public:
  bool ForEachKeyValue(
      nostd::function_ref<bool(SpanContext, const opentelemetry::common::KeyValueIterable &)>
      /* callback */) const noexcept override
  {
    return true;
  }

  size_t size() const noexcept override { return 0; };
};

}  // namespace trace
OPENTELEMETRY_END_NAMESPACE
