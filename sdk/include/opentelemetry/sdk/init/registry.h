// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <map>
#include <string>

#include "opentelemetry/version.h"

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

class OtlpPushMetricExporterBuilder;
class ConsolePushMetricExporterBuilder;
class ExtensionPushMetricExporterBuilder;
class PrometheusPullMetricExporterBuilder;
class ExtensionPullMetricExporterBuilder;

class OtlpLogRecordExporterBuilder;
class ConsoleLogRecordExporterBuilder;
class ExtensionLogRecordExporterBuilder;
class ExtensionLogRecordProcessorBuilder;

class Registry
{
public:
  Registry();
  ~Registry() = default;

  /* Core optional components. */

  const OtlpSpanExporterBuilder *GetOtlpHttpSpanBuilder() { return m_otlp_http_span_builder; }
  void SetOtlpHttpSpanBuilder(const OtlpSpanExporterBuilder *builder)
  {
    m_otlp_http_span_builder = builder;
  }

  const OtlpSpanExporterBuilder *GetOtlpGrpcSpanBuilder() { return m_otlp_grpc_span_builder; }
  void SetOtlpGrpcSpanBuilder(const OtlpSpanExporterBuilder *builder)
  {
    m_otlp_grpc_span_builder = builder;
  }

  const ConsoleSpanExporterBuilder *GetConsoleSpanBuilder() { return m_console_span_builder; }
  void SetConsoleSpanBuilder(const ConsoleSpanExporterBuilder *builder)
  {
    m_console_span_builder = builder;
  }

  const ZipkinSpanExporterBuilder *GetZipkinSpanBuilder() { return m_zipkin_span_builder; }
  void SetZipkinSpanBuilder(const ZipkinSpanExporterBuilder *builder)
  {
    m_zipkin_span_builder = builder;
  }

  const OtlpPushMetricExporterBuilder *GetOtlpHttpPushMetricExporterBuilder()
  {
    return m_otlp_http_push_metric_builder;
  }
  void SetOtlpHttpPushMetricExporterBuilder(const OtlpPushMetricExporterBuilder *builder)
  {
    m_otlp_http_push_metric_builder = builder;
  }

  const OtlpPushMetricExporterBuilder *GetOtlpGrpcPushMetricExporterBuilder()
  {
    return m_otlp_grpc_push_metric_builder;
  }
  void SetOtlpGrpcPushMetricExporterBuilder(const OtlpPushMetricExporterBuilder *builder)
  {
    m_otlp_grpc_push_metric_builder = builder;
  }

  const ConsolePushMetricExporterBuilder *GetConsolePushMetricExporterBuilder()
  {
    return m_console_metric_builder;
  }
  void SetConsolePushMetricExporterBuilder(const ConsolePushMetricExporterBuilder *builder)
  {
    m_console_metric_builder = builder;
  }

  const PrometheusPullMetricExporterBuilder *GetPrometheusPullMetricExporterBuilder()
  {
    return m_prometheus_metric_builder;
  }
  void SetPrometheusPullMetricExporterBuilder(const PrometheusPullMetricExporterBuilder *builder)
  {
    m_prometheus_metric_builder = builder;
  }

  const OtlpLogRecordExporterBuilder *GetOtlpHttpLogRecordBuilder()
  {
    return m_otlp_http_log_record_builder;
  }
  void SetOtlpHttpLogRecordBuilder(const OtlpLogRecordExporterBuilder *builder)
  {
    m_otlp_http_log_record_builder = builder;
  }

  const OtlpLogRecordExporterBuilder *GetOtlpGrpcLogRecordBuilder()
  {
    return m_otlp_grpc_log_record_builder;
  }
  void SetOtlpGrpcLogRecordBuilder(const OtlpLogRecordExporterBuilder *builder)
  {
    m_otlp_grpc_log_record_builder = builder;
  }

  const ConsoleLogRecordExporterBuilder *GetConsoleLogRecordBuilder()
  {
    return m_console_log_record_builder;
  }
  void SetConsoleLogRecordBuilder(const ConsoleLogRecordExporterBuilder *builder)
  {
    m_console_log_record_builder = builder;
  }

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

  const ExtensionPushMetricExporterBuilder *GetExtensionPushMetricExporterBuilder(
      const std::string &name);
  void AddExtensionPushMetricExporterBuilder(const std::string &name,
                                             ExtensionPushMetricExporterBuilder *builder);

  const ExtensionPullMetricExporterBuilder *GetExtensionPullMetricExporterBuilder(
      const std::string &name);
  void AddExtensionPullMetricExporterBuilder(const std::string &name,
                                             ExtensionPullMetricExporterBuilder *builder);

  const ExtensionLogRecordExporterBuilder *GetExtensionLogRecordExporterBuilder(
      const std::string &name);
  void AddExtensionLogRecordExporterBuilder(const std::string &name,
                                            ExtensionLogRecordExporterBuilder *builder);

  const ExtensionLogRecordProcessorBuilder *GetExtensionLogRecordProcessorBuilder(
      const std::string &name);
  void AddExtensionLogRecordProcessorBuilder(const std::string &name,
                                             ExtensionLogRecordProcessorBuilder *builder);

private:
  const OtlpSpanExporterBuilder *m_otlp_http_span_builder{nullptr};
  const OtlpSpanExporterBuilder *m_otlp_grpc_span_builder{nullptr};
  const ConsoleSpanExporterBuilder *m_console_span_builder{nullptr};
  const ZipkinSpanExporterBuilder *m_zipkin_span_builder{nullptr};

  const OtlpPushMetricExporterBuilder *m_otlp_http_push_metric_builder{nullptr};
  const OtlpPushMetricExporterBuilder *m_otlp_grpc_push_metric_builder{nullptr};
  const ConsolePushMetricExporterBuilder *m_console_metric_builder{nullptr};
  const PrometheusPullMetricExporterBuilder *m_prometheus_metric_builder{nullptr};

  const OtlpLogRecordExporterBuilder *m_otlp_http_log_record_builder{nullptr};
  const OtlpLogRecordExporterBuilder *m_otlp_grpc_log_record_builder{nullptr};
  const ConsoleLogRecordExporterBuilder *m_console_log_record_builder{nullptr};

  std::map<std::string, TextMapPropagatorBuilder *> m_propagator_builders;
  std::map<std::string, ExtensionSamplerBuilder *> m_sampler_builders;
  std::map<std::string, ExtensionSpanExporterBuilder *> m_span_exporter_builders;
  std::map<std::string, ExtensionSpanProcessorBuilder *> m_span_processor_builders;
  std::map<std::string, ExtensionPushMetricExporterBuilder *> m_push_metric_exporter_builders;
  std::map<std::string, ExtensionPullMetricExporterBuilder *> m_pull_metric_exporter_builders;
  std::map<std::string, ExtensionLogRecordExporterBuilder *> m_log_record_exporter_builders;
  std::map<std::string, ExtensionLogRecordProcessorBuilder *> m_log_record_processor_builders;
};

}  // namespace init
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE