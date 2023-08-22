// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/metrics/instruments.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

class OPENTELEMETRY_SDK_METRICS_EXPORT InstrumentSelector;

class OPENTELEMETRY_SDK_METRICS_EXPORT InstrumentSelectorFactory
{
public:
  static std::unique_ptr<InstrumentSelector> Create(
      opentelemetry::sdk::metrics::InstrumentType instrument_type,
      opentelemetry::nostd::string_view name,
      opentelemetry::nostd::string_view unit);
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
