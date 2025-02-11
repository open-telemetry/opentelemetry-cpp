// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
class MeterConfig
{
public:
  bool operator==(const MeterConfig &other) const noexcept;

  bool IsEnabled() const noexcept;

  static MeterConfig Disabled();

  static MeterConfig Enabled();

  static MeterConfig Default();

private:
  explicit MeterConfig(const bool disabled = false) : disabled_(disabled) {}
  bool disabled_;
  static const MeterConfig kDefaultConfig;
  static const MeterConfig kDisabledConfig;
};
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE