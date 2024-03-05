// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "opentelemetry/sdk/configuration/selector_configuration.h"
#include "opentelemetry/sdk/configuration/stream_configuration.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

class ViewConfiguration
{
public:
  ViewConfiguration()  = default;
  ~ViewConfiguration() = default;

  std::unique_ptr<SelectorConfiguration> selector;
  std::unique_ptr<StreamConfiguration> stream;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
