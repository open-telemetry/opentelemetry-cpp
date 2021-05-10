#pragma once
#include <string>
#include "opentelemetry/context/context.h"
#include "opentelemetry/context/propagation/global_propagator.h"
#include "opentelemetry/context/propagation/text_map_propagator.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/trace/provider.h"


namespace {
template <typename T>
class gRPCMapCarrier : public opentelemetry::context::propagation::TextMapCarrier
{
public:
  gRPCMapCarrier<T>(T &headers) : headers_(headers) {}
  gRPCMapCarrier() = default;
  virtual nostd::string_view Get(nostd::string_view key) const noexcept override
  {
    std::string key_to_compare = key.data();
    if (key == opentelemetry::trace::propagation::kTraceParent)
    {
      key_to_compare = "Traceparent";
    }
    else if (key == opentelemetry::trace::propagation::kTraceState)
    {
      key_to_compare = "Tracestate";
    }
    auto it = headers_.find(key_to_compare);
    if (it != headers_.end())
    {
      return it->second;
    }
    return "";
  }

  virtual void Set(nostd::string_view key, nostd::string_view value) noexcept override
  {
    headers_.insert(std::pair<std::string, std::string>(std::string(key), std::string(value)));
  }

  T headers_;
};

} // namespace