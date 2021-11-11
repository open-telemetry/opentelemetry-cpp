// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/version/version.h"
#include "opentelemetry/logs/provider.h"
#include "opentelemetry/trace/provider.h"
#include <map>
#include <string>

namespace logs = opentelemetry::logs;
namespace trace = opentelemetry::trace;
namespace nostd = opentelemetry::nostd;

namespace
{
nostd::shared_ptr<trace::Tracer> get_tracer()
{
	  auto provider = trace::Provider::GetTracerProvider();
	  return provider->GetTracer("foo_library", OPENTELEMETRY_SDK_VERSION);
}

nostd::shared_ptr<logs::Logger> get_logger()
{
  auto provider = logs::Provider::GetLoggerProvider();
  return provider->GetLogger("foo_library_logger", nostd::span<nostd::string_view>());
}
}

void foo_library()
{
  auto span  = get_tracer()->StartSpan("span 1");
  auto scoped_span = trace::Scope(get_tracer()->StartSpan("foo_library"));
  auto ctx = span->GetContext();
  auto logger = get_logger();
  logger->Log(opentelemetry::logs::Severity::kDebug,
          "name",
          "body",
          std::map<std::string, std::string>(),
          std::map<std::string, std::string>(),
          ctx.trace_id(),
          ctx.span_id(),
          ctx.trace_flags(),
          opentelemetry::common::SystemTimestamp());
}
