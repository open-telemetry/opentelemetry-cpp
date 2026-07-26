// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <chrono>
#include <memory>

#include "opentelemetry/sdk/logs/processor.h"
#include "opentelemetry/sdk/logs/recordable.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{

/**
 * A log record processor that bridges log records representing events (i.e. log records with a
 * non-empty event name) onto the current active span as span events.
 *
 * A log record is bridged to a span event if and only if:
 *  - the log record has a non-empty event name,
 *  - the log record carries a valid trace id and span id,
 *  - the current active span (from the ambient context at the time OnEmit is called) is
 *    recording, and
 *  - the current active span's trace id and span id match those on the log record.
 *
 * This processor does not export log records and does not stop the log record from continuing
 * through the rest of the configured processing pipeline.
 */
class EventToSpanEventBridgeProcessor : public LogRecordProcessor
{
public:
  EventToSpanEventBridgeProcessor()                                                   = default;
  EventToSpanEventBridgeProcessor(const EventToSpanEventBridgeProcessor &)            = delete;
  EventToSpanEventBridgeProcessor(EventToSpanEventBridgeProcessor &&)                 = delete;
  EventToSpanEventBridgeProcessor &operator=(const EventToSpanEventBridgeProcessor &) = delete;
  EventToSpanEventBridgeProcessor &operator=(EventToSpanEventBridgeProcessor &&)      = delete;
  ~EventToSpanEventBridgeProcessor() override                                         = default;

  std::unique_ptr<Recordable> MakeRecordable() noexcept override;

  void OnEmit(std::unique_ptr<Recordable> &&record) noexcept override;

  bool ForceFlush(
      std::chrono::microseconds timeout = (std::chrono::microseconds::max)()) noexcept override;

  bool Shutdown(
      std::chrono::microseconds timeout = (std::chrono::microseconds::max)()) noexcept override;

  bool HasEnabledFilter() const noexcept override { return false; }
};

}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
