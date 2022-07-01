// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "opentelemetry/sdk/trace/id_generator.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{

class RandomIdGeneratorFactory
{
public:
  static std::unique_ptr<IdGenerator> Build();
};

}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
