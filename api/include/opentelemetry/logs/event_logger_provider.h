// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifdef ENABLE_LOGS_PREVIEW

#  include "opentelemetry/common/key_value_iterable.h"
#  include "opentelemetry/common/key_value_iterable_view.h"
#  include "opentelemetry/logs/event_logger.h"
#  include "opentelemetry/logs/logger.h"
#  include "opentelemetry/nostd/shared_ptr.h"
#  include "opentelemetry/nostd/string_view.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace logs
{
/**
 * Creates new EventLogger instances.
 */
class EventLoggerProvider
{
public:
  virtual ~EventLoggerProvider() = default;

  /**
   * Creates a named EventLogger instance.
   *
   */

  virtual nostd::shared_ptr<EventLogger> CreateEventLogger(
      nostd::shared_ptr<Logger> delegate_logger,
      nostd::string_view event_domain) noexcept = 0;
};
}  // namespace logs
OPENTELEMETRY_END_NAMESPACE
#endif
