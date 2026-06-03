// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/logs/log_record.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/logs/log_record_limits.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace resource
{
class Resource;
}  // namespace resource

namespace instrumentationscope
{
class InstrumentationScope;
}  // namespace instrumentationscope

namespace logs
{
class MultiRecordable;

/**
 * Maintains a representation of a log in a format that can be processed by a recorder.
 *
 * This class is thread-compatible.
 */

class Recordable : public opentelemetry::logs::LogRecord
{
  friend class MultiRecordable;

public:
  void SetLogRecordLimits(const LogRecordLimits &limits) noexcept { limits_ = limits; }

  void SetAttribute(opentelemetry::nostd::string_view key,
                    const opentelemetry::common::AttributeValue &value) noexcept final
  {
    SetAttributeImpl(key, value);
  }

  /**
   * Set Resource of this log
   * @param resource the resource to set
   */
  virtual void SetResource(const opentelemetry::sdk::resource::Resource &resource) noexcept = 0;

  /**
   * Set instrumentation_scope for this log.
   * @param instrumentation_scope the instrumentation scope to set
   */
  virtual void SetInstrumentationScope(
      const opentelemetry::sdk::instrumentationscope::InstrumentationScope
          &instrumentation_scope) noexcept = 0;

protected:
  virtual void SetAttributeImpl(opentelemetry::nostd::string_view key,
                                const opentelemetry::common::AttributeValue &value) noexcept = 0;

private:
  LogRecordLimits limits_;
};

}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
