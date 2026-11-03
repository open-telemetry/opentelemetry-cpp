// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <new>

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
    explicit Cache(bool &destroyed) : destroyed_(destroyed) {}
    ~Cache() { destroyed_ = true; }

    Properties fields;
    bool busy = false;
    bool &destroyed_;
  };

  static Cache *ThreadCache()
  {
    // Trivial TLS state remains valid while other thread-local destructors run.
    static thread_local bool destroyed = false;
    if (destroyed)
    {
      return nullptr;
    }
    static thread_local Cache cache(destroyed);
    return &cache;
  }

public:
  explicit SpanEndProperties(bool reusable)
  {
    if (reusable)
    {
      auto *candidate = ThreadCache();
      if (candidate != nullptr && !candidate->busy)
      {
        candidate->busy = true;
        cache_          = candidate;
        fields_         = &candidate->fields;
        // ID overrides are only added when saved IDs are absent or modified.
        fields_->erase(ETW_FIELD_SPAN_ID);
        fields_->erase(ETW_FIELD_TRACE_ID);
        return;
      }
    }
    fields_ = new (fallback_) Properties;
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
  alignas(Properties) unsigned char fallback_[sizeof(Properties)];
  Cache *cache_       = nullptr;
  Properties *fields_ = nullptr;
};
}  // namespace detail
}  // namespace etw
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
