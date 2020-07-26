#pragma once
#include "opentelemetry/version.h"
#include "opentelemetry/metrics/instrument.h"
#include "opentelemetry/sdk/metrics/record.h"
#include "opentelemetry/nostd/string_view.h"

#include <unordered_map>
#include <string>
#include <iostream>

namespace sdkmetrics = opentelemetry::sdk::metrics;

OPENTELEMETRY_BEGIN_NAMESPACE

namespace sdk
{
namespace metrics
{

class MetricsProcessor
{
public:
  virtual ~MetricsProcessor() = default;

  virtual std::vector<sdkmetrics::Record> CheckpointSelf() noexcept = 0;

  virtual void FinishedCollection() noexcept = 0;

  virtual void process(sdkmetrics::Record record) noexcept = 0;
};

}
}

OPENTELEMETRY_END_NAMESPACE