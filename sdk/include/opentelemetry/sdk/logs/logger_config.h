// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{
class LoggerConfig
{
public:
  bool operator==(const LoggerConfig &other) const noexcept;

  bool IsEnabled() const noexcept;

  static LoggerConfig Enabled();

  static LoggerConfig Disabled();

  static LoggerConfig Default();

private:
  explicit LoggerConfig(const bool disabled = false) : disabled_(disabled) {}

  bool disabled_;
  static const LoggerConfig kDisabledConfig;
  static const LoggerConfig kDefaultConfig;

};
} // namespace logs
} // namespace sdk
OPENTELEMETRY_END_NAMESPACE
