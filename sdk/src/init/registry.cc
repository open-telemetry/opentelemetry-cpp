// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/common/global_log_handler.h"

#include "opentelemetry/sdk/init/registry.h"
#include "opentelemetry/sdk/init/otlp_span_exporter_builder.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace init
{

static const OtlpSpanExporterBuilder *otlp_singleton = nullptr;
static const ConsoleSpanExporterBuilder *console_singleton = nullptr;
static const ZipkinSpanExporterBuilder *zipkin_singleton = nullptr;

const OtlpSpanExporterBuilder* Registry::GetOtlpBuilder() {
  return otlp_singleton;
}

void Registry::SetOtlpBuilder(const OtlpSpanExporterBuilder * builder)
{
  otlp_singleton = builder;
}

const ConsoleSpanExporterBuilder* Registry::GetConsoleBuilder() {
  return console_singleton;
}

void Registry::SetConsoleBuilder(const ConsoleSpanExporterBuilder * builder)
{
  console_singleton = builder;
}

const ZipkinSpanExporterBuilder* Registry::GetZipkinBuilder() {
  return zipkin_singleton;
}

void Registry::SetZipkinBuilder(const ZipkinSpanExporterBuilder * builder)
{
  zipkin_singleton = builder;
}


}  // namespace init
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
