// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "opentelemetry/version.h"
#include "opentelemetry/sdk/trace/id_generator.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{

/**
 * Factory class for RandomIdGenerator.
 */
class OPENTELEMETRY_EXPORT_TYPE RandomIdGeneratorFactory
{
public:
  /**
   * Create a RandomIdGenerator.
   */
  static std::unique_ptr<IdGenerator> Create();
};

}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
