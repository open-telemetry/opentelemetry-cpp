// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/version.h"

#include <map>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace init
{
class OtlpSpanExporterBuilder;
class ConsoleSpanExporterBuilder;
class ZipkinSpanExporterBuilder;
class TextMapPropagatorBuilder;

class Registry
{
public:
  Registry();
  ~Registry() = default;

  const OtlpSpanExporterBuilder *GetOtlpBuilder() { return m_otlp_builder; }
  void SetOtlpBuilder(const OtlpSpanExporterBuilder *builder) { m_otlp_builder = builder; }

  const ConsoleSpanExporterBuilder *GetConsoleBuilder() { return m_console_builder; }
  void SetConsoleBuilder(const ConsoleSpanExporterBuilder *builder) { m_console_builder = builder; }

  const ZipkinSpanExporterBuilder *GetZipkinBuilder() { return m_zipkin_builder; }
  void SetZipkinBuilder(const ZipkinSpanExporterBuilder *builder) { m_zipkin_builder = builder; }

  const TextMapPropagatorBuilder *GetTextMapPropagatorBuilder(const std::string &name);
  void AddTextMapPropagatorBuilder(const std::string &name, TextMapPropagatorBuilder *builder);

private:
  const OtlpSpanExporterBuilder *m_otlp_builder{nullptr};
  const ConsoleSpanExporterBuilder *m_console_builder{nullptr};
  const ZipkinSpanExporterBuilder *m_zipkin_builder{nullptr};

  std::map<std::string, TextMapPropagatorBuilder *> m_propagator_builders;
};

}  // namespace init
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
