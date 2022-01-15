// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifdef ENABLE_LOGS_PREVIEW

// Please refer to provider.h for documentation on how to obtain a Logger object.
//
// This file is part of the internal implementation of OpenTelemetry. Nothing in this file should be
// used directly. Please refer to logger.h for documentation on these interfaces.

#  include <memory>

#  include "opentelemetry/common/attribute_value.h"
#  include "opentelemetry/common/key_value_iterable.h"
#  include "opentelemetry/common/timestamp.h"
#  include "opentelemetry/context/runtime_context.h"
#  include "opentelemetry/logs/logger.h"
#  include "opentelemetry/logs/logger_provider.h"
#  include "opentelemetry/logs/severity.h"
#  include "opentelemetry/nostd/shared_ptr.h"
#  include "opentelemetry/nostd/span.h"
#  include "opentelemetry/nostd/string_view.h"
#  include "opentelemetry/nostd/unique_ptr.h"
#  include "opentelemetry/trace/span_id.h"
#  include "opentelemetry/trace/trace_flags.h"
#  include "opentelemetry/trace/trace_id.h"
#  include "opentelemetry/version.h"

#  include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace logs
{
/**
 * No-op implementation of Logger. This class should not be used directly. It should only be
 * instantiated using a LoggerProvider's GetLogger() call.
 */
class NoopLogger final : public Logger
{
public:
  const nostd::string_view GetName() noexcept override { return "noop logger"; }

  void Log(Severity severity,
           nostd::string_view name,
           nostd::string_view body,
           const common::KeyValueIterable &attributes,
           trace::TraceId trace_id,
           trace::SpanId span_id,
           trace::TraceFlags trace_flags,
           common::SystemTimestamp timestamp) noexcept override
  {}
};

/**
 * No-op implementation of a LoggerProvider.
 */
class NoopLoggerProvider final : public opentelemetry::logs::LoggerProvider
{
public:
  NoopLoggerProvider()
      : logger_{
            nostd::shared_ptr<opentelemetry::logs::NoopLogger>(new opentelemetry::logs::NoopLogger)}
  {}

  nostd::shared_ptr<Logger> GetLogger(nostd::string_view logger_name,
                                      nostd::string_view options,
                                      nostd::string_view library_name,
                                      nostd::string_view library_version = "",
                                      nostd::string_view schema_url      = "") override
  {
    return logger_;
  }

  nostd::shared_ptr<Logger> GetLogger(nostd::string_view logger_name,
                                      nostd::span<nostd::string_view> args,
                                      nostd::string_view library_name,
                                      nostd::string_view library_version = "",
                                      nostd::string_view schema_url      = "") override
  {
    return logger_;
  }

private:
  nostd::shared_ptr<opentelemetry::logs::Logger> logger_;
};
}  // namespace logs
OPENTELEMETRY_END_NAMESPACE
#endif
