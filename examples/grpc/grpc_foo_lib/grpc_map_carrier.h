#pragma once
#include "opentelemetry/context/context.h"
#include "opentelemetry/context/propagation/text_map_propagator.h"
#include "opentelemetry/nostd/string_view.h"

class gRPCMapCarrier : public opentelemetry::context::propagation::TextMapCarrie
{
private:
  std::map<std::string, std::string> headers_;

public:
  virtual opentelemetry::nostd::string_view Get(
      opentelemetry::nostd::string_view key) const noexcept override
  {
    auto it = headers_.find(std::string(key));
    if (it != headers_.end())
    {
      return opentelemetry::nostd::string_view(it->second);
    }
    return "";
  }

  virtual void Set(opentelemetry::nostd::string_view key,
                   opentelemetry::nostd::string_view value) noexcept override
  {
    headers_[std::string(key)] = std::string(value);
  }
};
