// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace init
{
class OtlpSpanExporterBuilder;
class ConsoleSpanExporterBuilder;
class ZipkinSpanExporterBuilder;

class Registry
{
public:
  Registry()  = default;
  ~Registry() = default;

  const OtlpSpanExporterBuilder *GetOtlpBuilder() { return m_otlp_builder; }
  void SetOtlpBuilder(const OtlpSpanExporterBuilder *builder) { m_otlp_builder = builder; }

  const ConsoleSpanExporterBuilder *GetConsoleBuilder() { return m_console_builder; }
  void SetConsoleBuilder(const ConsoleSpanExporterBuilder *builder) { m_console_builder = builder; }

  const ZipkinSpanExporterBuilder *GetZipkinBuilder() { return m_zipkin_builder; }
  void SetZipkinBuilder(const ZipkinSpanExporterBuilder *builder) { m_zipkin_builder = builder; }

private:
  const OtlpSpanExporterBuilder *m_otlp_builder{nullptr};
  const ConsoleSpanExporterBuilder *m_console_builder{nullptr};
  const ZipkinSpanExporterBuilder *m_zipkin_builder{nullptr};
};

}  // namespace init
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
