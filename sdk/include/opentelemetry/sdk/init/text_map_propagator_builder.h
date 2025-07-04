// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/context/propagation/text_map_propagator.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace init
{

class TextMapPropagatorBuilder
{
public:
  TextMapPropagatorBuilder()                                                 = default;
  TextMapPropagatorBuilder(TextMapPropagatorBuilder &&)                      = default;
  TextMapPropagatorBuilder(const TextMapPropagatorBuilder &)                 = default;
  TextMapPropagatorBuilder &operator=(TextMapPropagatorBuilder &&)           = default;
  TextMapPropagatorBuilder &operator=(const TextMapPropagatorBuilder &other) = default;
  virtual ~TextMapPropagatorBuilder()                                        = default;

  virtual std::unique_ptr<opentelemetry::context::propagation::TextMapPropagator> Build() const = 0;
};

}  // namespace init
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
