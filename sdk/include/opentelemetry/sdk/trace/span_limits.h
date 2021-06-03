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
#include "opentelemetry/common/spin_lock_mutex.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
/**
 * Collection of span limits configurations.
 */
class SpanLimits
{
public:
  /**
   * Setter methods of configuration params
   */
  static void SetAttributeCountLimit(int attributeCountLimit)
  {
    std::lock_guard<opentelemetry::common::SpinLockMutex> guard(GetLock());
    GetSpanLimits()->AttributeCountLimit = attributeCountLimit;
  }

  static void SetEventCountLimit(int eventCountLimit)
  {
    std::lock_guard<opentelemetry::common::SpinLockMutex> guard(GetLock());
    GetSpanLimits()->EventCountLimit = eventCountLimit;
  }

  static void SetLinkCountLimit(int linkCountLimit)
  {
    std::lock_guard<opentelemetry::common::SpinLockMutex> guard(GetLock());
    GetSpanLimits()->LinkCountLimit = linkCountLimit;
  }

  static void SetAttributePerEventCountLimit(int attributePerEventCountLimit)
  {
    std::lock_guard<opentelemetry::common::SpinLockMutex> guard(GetLock());
    GetSpanLimits()->AttributePerEventCountLimit = attributePerEventCountLimit;
  }

  static void SetAttributePerLinkCountLimit(int attributePerLinkCountLimit)
  {
    std::lock_guard<opentelemetry::common::SpinLockMutex> guard(GetLock());
    GetSpanLimits()->AttributePerLinkCountLimit = attributePerLinkCountLimit;
  }

  /**
   * Getter methods of configuration params
   */
  static int GetAttributeCountLimit()
  {
    std::lock_guard<opentelemetry::common::SpinLockMutex> guard(GetLock());
    return GetSpanLimits()->AttributeCountLimit;
  }

  static int GetEventCountLimit()
  {
    std::lock_guard<opentelemetry::common::SpinLockMutex> guard(GetLock());
    return GetSpanLimits()->EventCountLimit;
  }

  static int GetLinkCountLimit()
  {
    std::lock_guard<opentelemetry::common::SpinLockMutex> guard(GetLock());
    return GetSpanLimits()->LinkCountLimit;
  }

  static int GetAttributePerEventCountLimit()
  {
    std::lock_guard<opentelemetry::common::SpinLockMutex> guard(GetLock());
    return GetSpanLimits()->AttributePerEventCountLimit;
  }

  static int GetAttributePerLinkCountLimit()
  {
    std::lock_guard<opentelemetry::common::SpinLockMutex> guard(GetLock());
    return GetSpanLimits()->AttributePerLinkCountLimit;
  }

private:
  static nostd::shared_ptr<SpanLimits> &GetSpanLimits() noexcept
  {
    static nostd::shared_ptr<SpanLimits> spanLimits(new SpanLimits);
    return spanLimits;
  }

  static opentelemetry::common::SpinLockMutex &GetLock() noexcept
  {
    static opentelemetry::common::SpinLockMutex lock;
    return lock;
  }

  int AttributeCountLimit         = 128;
  int EventCountLimit             = 128;
  int LinkCountLimit              = 128;
  int AttributePerEventCountLimit = 128;
  int AttributePerLinkCountLimit  = 128;
};
}  // namespace trace
}  // namespace sdk

OPENTELEMETRY_END_NAMESPACE