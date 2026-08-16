// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

// Propagator test helpers for SDK configuration tests.

#pragma once

#include <map>
#include <string>

#include "opentelemetry/context/propagation/text_map_propagator.h"
#include "opentelemetry/nostd/string_view.h"

namespace config_test
{

// ---------------------------------------------------------------------------
// TextMapCarrier for propagator tests.

class MapCarrier : public opentelemetry::context::propagation::TextMapCarrier
{
public:
  opentelemetry::nostd::string_view Get(
      opentelemetry::nostd::string_view key) const noexcept override
  {
    auto it = map_.find(std::string(key));
    return it != map_.end() ? opentelemetry::nostd::string_view(it->second) : "";
  }
  void Set(opentelemetry::nostd::string_view key,
           opentelemetry::nostd::string_view value) noexcept override
  {
    map_[std::string(key)] = std::string(value);
  }

  const std::map<std::string, std::string> &map() const { return map_; }

private:
  std::map<std::string, std::string> map_;
};

}  // namespace config_test
