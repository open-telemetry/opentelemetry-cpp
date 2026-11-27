// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "opentelemetry/sdk/configuration/always_record_sampler_configuration.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
class Sampler;
}  // namespace trace

namespace configuration
{

class AlwaysRecordSamplerBuilder
{
public:
  AlwaysRecordSamplerBuilder()                                                   = default;
  AlwaysRecordSamplerBuilder(AlwaysRecordSamplerBuilder &&)                      = default;
  AlwaysRecordSamplerBuilder(const AlwaysRecordSamplerBuilder &)                 = default;
  AlwaysRecordSamplerBuilder &operator=(AlwaysRecordSamplerBuilder &&)           = default;
  AlwaysRecordSamplerBuilder &operator=(const AlwaysRecordSamplerBuilder &other) = default;
  virtual ~AlwaysRecordSamplerBuilder()                                          = default;

  virtual std::unique_ptr<opentelemetry::sdk::trace::Sampler> Build(
      const opentelemetry::sdk::configuration::AlwaysRecordSamplerConfiguration *model) const = 0;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
