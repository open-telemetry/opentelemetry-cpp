// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>
#include <mutex>

#include "opentelemetry/nostd/span.h"
#include "opentelemetry/sdk/logs/recordable.h"
#include "opentelemetry/sdk/trace/recordable.h"
#include "opentelemetry/version.h"

namespace opentelemetry
{
namespace proto
{
namespace collector
{

namespace logs
{
namespace v1
{
class ExportLogsServiceRequest;
}
}  // namespace logs
namespace trace
{
namespace v1
{
class ExportTraceServiceRequest;
}
}  // namespace trace

}  // namespace collector
}  // namespace proto
}  // namespace opentelemetry

namespace google
{
namespace protobuf
{
class Arena;
}  // namespace protobuf
}  // namespace google

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{
/**
 * The OtlpRecordableUtils contains utility functions for OTLP recordable
 */
class OtlpRecordableUtils
{
public:
  /**
   * Populate request with the spans, grouped by resource and instrumentation scope.
   *
   * Each element of spans must be an OtlpRecordable. A recordable whose span message is on the
   * same Arena as request is moved from: request takes the span message itself rather than a
   * copy, so afterwards request refers to the recordable's message, and the recordable must not
   * be modified or passed to PopulateRequest again. It can still be destroyed, in any order
   * with request, since neither owns the message, the Arena does. Every other span is copied.
   */
  static void PopulateRequest(
      const nostd::span<std::unique_ptr<opentelemetry::sdk::trace::Recordable>> &spans,
      proto::collector::trace::v1::ExportTraceServiceRequest *request) noexcept;

  /**
   * Populate request with the log records, grouped by resource and instrumentation scope.
   *
   * Each element of logs must be an OtlpLogRecordable. A recordable whose log record message is on
   * the same Arena as request is moved from, with the same rules as the spans overload above.
   * Every other log record is copied.
   */
  static void PopulateRequest(
      const nostd::span<std::unique_ptr<opentelemetry::sdk::logs::Recordable>> &logs,
      proto::collector::logs::v1::ExportLogsServiceRequest *request) noexcept;
};

/**
 * The Arena an OTLP exporter shares across the recordables it creates between two exports.
 *
 * MakeRecordable() creates each recordable on Get(), and Export() takes the Arena with Rotate()
 * and creates the request on it, so PopulateRequest() moves the messages of the recordables
 * created since the previous export into the request instead of copying them. The Arena is
 * reference counted: the exporter, every recordable created on it and the request built on it
 * each hold a reference, and the last one to go destroys it. That is what keeps a recordable
 * valid when it was created before an export and is exported by a later one, and what keeps
 * destruction single threaded, since the Arena is only destroyed once nothing can still
 * allocate on it.
 *
 * Get(), Rotate() and Release() may be called concurrently.
 */
class OtlpRecordableArena
{
public:
  OtlpRecordableArena();

  /** Returns the current Arena, or nullptr after Release(). */
  std::shared_ptr<google::protobuf::Arena> Get();

  /**
   * Returns the current Arena and replaces it with a new one. After Release(), returns a new
   * Arena and keeps none. Never returns nullptr.
   */
  std::shared_ptr<google::protobuf::Arena> Rotate();

  /**
   * Drops the reference to the current Arena, which lives on for as long as a recordable or a
   * request still holds it. Called by an exporter's Shutdown(): a shut down exporter no longer
   * exports, so nothing would rotate the Arena any more, and recordables that keep being created
   * on it would only make it grow. After this, Get() returns nullptr, so every recordable gets an
   * Arena of its own that is freed with it.
   */
  void Release();

private:
  static std::shared_ptr<google::protobuf::Arena> MakeArena();

  std::mutex lock_;
  std::shared_ptr<google::protobuf::Arena> arena_;
};
}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
