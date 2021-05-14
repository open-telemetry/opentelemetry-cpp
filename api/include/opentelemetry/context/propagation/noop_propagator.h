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

#include "opentelemetry/context/propagation/text_map_propagator.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace context
{
namespace propagation
{

/**
 * No-op implementation TextMapPropagator
 */
class NoOpPropagator : public TextMapPropagator
{
public:
  /** Noop extract function does nothing and returns the input context */
  context::Context Extract(const TextMapCarrier & /*carrier*/,
                           context::Context &context) noexcept override
  {
    return context;
  }

  /** Noop inject function does nothing */
  void Inject(TextMapCarrier & /*carrier*/, const context::Context &context) noexcept override {}
};
}  // namespace propagation
}  // namespace context
OPENTELEMETRY_END_NAMESPACE