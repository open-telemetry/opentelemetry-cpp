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
#ifdef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/metrics/instrument.h"
#  include "opentelemetry/nostd/string_view.h"
#  include "opentelemetry/sdk/metrics/record.h"
#  include "opentelemetry/version.h"

#  include <iostream>
#  include <string>
#  include <unordered_map>

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

}  // namespace metrics
}  // namespace sdk

OPENTELEMETRY_END_NAMESPACE
#endif
