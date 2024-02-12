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
class ExtensionSamplerBuilder;
class ExtensionSpanExporterBuilder;
class ExtensionSpanProcessorBuilder;

class Registry
{
public:
  Registry();
  ~Registry() = default;

  /* Core optional components. */

  const OtlpSpanExporterBuilder *GetOtlpBuilder() { return m_otlp_builder; }
  void SetOtlpBuilder(const OtlpSpanExporterBuilder *builder) { m_otlp_builder = builder; }

  const ConsoleSpanExporterBuilder *GetConsoleBuilder() { return m_console_builder; }
  void SetConsoleBuilder(const ConsoleSpanExporterBuilder *builder) { m_console_builder = builder; }

  const ZipkinSpanExporterBuilder *GetZipkinBuilder() { return m_zipkin_builder; }
  void SetZipkinBuilder(const ZipkinSpanExporterBuilder *builder) { m_zipkin_builder = builder; }

  /* Extension points */

  const TextMapPropagatorBuilder *GetTextMapPropagatorBuilder(const std::string &name);
  void AddTextMapPropagatorBuilder(const std::string &name, TextMapPropagatorBuilder *builder);

  const ExtensionSamplerBuilder *GetExtensionSamplerBuilder(const std::string &name);
  void AddExtensionSamplerBuilder(const std::string &name, ExtensionSamplerBuilder *builder);

  const ExtensionSpanExporterBuilder *GetExtensionSpanExporterBuilder(const std::string &name);
  void AddExtensionSpanExporterBuilder(const std::string &name,
                                       ExtensionSpanExporterBuilder *builder);

  const ExtensionSpanProcessorBuilder *GetExtensionSpanProcessorBuilder(const std::string &name);
  void AddExtensionSpanProcessorBuilder(const std::string &name,
                                        ExtensionSpanProcessorBuilder *builder);

private:
  const OtlpSpanExporterBuilder *m_otlp_builder{nullptr};
  const ConsoleSpanExporterBuilder *m_console_builder{nullptr};
  const ZipkinSpanExporterBuilder *m_zipkin_builder{nullptr};

  std::map<std::string, TextMapPropagatorBuilder *> m_propagator_builders;
  std::map<std::string, ExtensionSamplerBuilder *> m_sampler_builders;
  std::map<std::string, ExtensionSpanExporterBuilder *> m_span_exporter_builders;
  std::map<std::string, ExtensionSpanProcessorBuilder *> m_span_processor_builders;
};

}  // namespace init
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
