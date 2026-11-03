// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <new>
#include <type_traits>

#include "opentelemetry/exporters/etw/etw_fields.h"
#include "opentelemetry/exporters/etw/etw_properties.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace etw
{
namespace detail
{
// Reuse only generated fields that are all overwritten before a single TLD write.
// Caller attributes must never be copied into the retained map.
class SpanEndProperties
{
  struct Cache
  {
    Properties fields;
    bool busy = false;
  };

  static Cache &ThreadCache()
  {
    static thread_local Cache cache;
    return cache;
  }

public:
  explicit SpanEndProperties(bool reusable)
  {
    if (reusable)
    {
      auto &candidate = ThreadCache();
      if (!candidate.busy)
      {
        candidate.busy = true;
        cache_         = &candidate;
        fields_        = &candidate.fields;
        // ID overrides are only added when saved IDs are absent or modified.
        fields_->erase(ETW_FIELD_SPAN_ID);
        fields_->erase(ETW_FIELD_TRACE_ID);
        return;
      }
    }
    fields_ = new (&fallback_) Properties;
  }

  ~SpanEndProperties()
  {
    if (cache_ != nullptr)
    {
      // Retain nodes, not span strings or vectors.
      for (auto &entry : *fields_)
      {
        entry.second = int32_t{0};
      }
      cache_->busy = false;
    }
    else
    {
      fields_->~Properties();
    }
  }

  SpanEndProperties(const SpanEndProperties &)            = delete;
  SpanEndProperties &operator=(const SpanEndProperties &) = delete;

  Properties &Get() { return *fields_; }

private:
  std::aligned_storage<sizeof(Properties), alignof(Properties)>::type fallback_;
  Cache *cache_       = nullptr;
  Properties *fields_ = nullptr;
};
}  // namespace detail
}  // namespace etw
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
