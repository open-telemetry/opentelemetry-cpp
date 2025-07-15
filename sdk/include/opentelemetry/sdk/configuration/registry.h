// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <map>
#include <string>

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{
class OtlpHttpSpanExporterBuilder;
class OtlpGrpcSpanExporterBuilder;
class OtlpFileSpanExporterBuilder;
class ConsoleSpanExporterBuilder;
class ZipkinSpanExporterBuilder;
class TextMapPropagatorBuilder;
class ExtensionSamplerBuilder;
class ExtensionSpanExporterBuilder;
class ExtensionSpanProcessorBuilder;

class OtlpHttpPushMetricExporterBuilder;
class OtlpGrpcPushMetricExporterBuilder;
class OtlpFilePushMetricExporterBuilder;
class ConsolePushMetricExporterBuilder;
class ExtensionPushMetricExporterBuilder;
class PrometheusPullMetricExporterBuilder;
class ExtensionPullMetricExporterBuilder;

class OtlpHttpLogRecordExporterBuilder;
class OtlpGrpcLogRecordExporterBuilder;
class OtlpFileLogRecordExporterBuilder;
class ConsoleLogRecordExporterBuilder;
class ExtensionLogRecordExporterBuilder;
class ExtensionLogRecordProcessorBuilder;

class Registry
{
public:
  Registry();
  Registry(Registry &&)                      = default;
  Registry(const Registry &)                 = default;
  Registry &operator=(Registry &&)           = default;
  Registry &operator=(const Registry &other) = default;

  ~Registry() = default;

  /* Core optional components. */

  const OtlpHttpSpanExporterBuilder *GetOtlpHttpSpanBuilder() const
  {
    return otlp_http_span_builder_;
  }
  void SetOtlpHttpSpanBuilder(const OtlpHttpSpanExporterBuilder *builder)
  {
    otlp_http_span_builder_ = builder;
  }

  const OtlpGrpcSpanExporterBuilder *GetOtlpGrpcSpanBuilder() const
  {
    return otlp_grpc_span_builder_;
  }
  void SetOtlpGrpcSpanBuilder(const OtlpGrpcSpanExporterBuilder *builder)
  {
    otlp_grpc_span_builder_ = builder;
  }

  const OtlpFileSpanExporterBuilder *GetOtlpFileSpanBuilder() const
  {
    return otlp_file_span_builder_;
  }
  void SetOtlpFileSpanBuilder(const OtlpFileSpanExporterBuilder *builder)
  {
    otlp_file_span_builder_ = builder;
  }

  const ConsoleSpanExporterBuilder *GetConsoleSpanBuilder() const { return console_span_builder_; }
  void SetConsoleSpanBuilder(const ConsoleSpanExporterBuilder *builder)
  {
    console_span_builder_ = builder;
  }

  const ZipkinSpanExporterBuilder *GetZipkinSpanBuilder() const { return zipkin_span_builder_; }
  void SetZipkinSpanBuilder(const ZipkinSpanExporterBuilder *builder)
  {
    zipkin_span_builder_ = builder;
  }

  const OtlpHttpPushMetricExporterBuilder *GetOtlpHttpPushMetricExporterBuilder() const
  {
    return otlp_http_push_metric_builder_;
  }
  void SetOtlpHttpPushMetricExporterBuilder(const OtlpHttpPushMetricExporterBuilder *builder)
  {
    otlp_http_push_metric_builder_ = builder;
  }

  const OtlpGrpcPushMetricExporterBuilder *GetOtlpGrpcPushMetricExporterBuilder() const
  {
    return otlp_grpc_push_metric_builder_;
  }
  void SetOtlpGrpcPushMetricExporterBuilder(const OtlpGrpcPushMetricExporterBuilder *builder)
  {
    otlp_grpc_push_metric_builder_ = builder;
  }

  const OtlpFilePushMetricExporterBuilder *GetOtlpFilePushMetricExporterBuilder() const
  {
    return otlp_file_push_metric_builder_;
  }
  void SetOtlpFilePushMetricExporterBuilder(const OtlpFilePushMetricExporterBuilder *builder)
  {
    otlp_file_push_metric_builder_ = builder;
  }

  const ConsolePushMetricExporterBuilder *GetConsolePushMetricExporterBuilder() const
  {
    return console_metric_builder_;
  }
  void SetConsolePushMetricExporterBuilder(const ConsolePushMetricExporterBuilder *builder)
  {
    console_metric_builder_ = builder;
  }

  const PrometheusPullMetricExporterBuilder *GetPrometheusPullMetricExporterBuilder() const
  {
    return prometheus_metric_builder_;
  }
  void SetPrometheusPullMetricExporterBuilder(const PrometheusPullMetricExporterBuilder *builder)
  {
    prometheus_metric_builder_ = builder;
  }

  const OtlpHttpLogRecordExporterBuilder *GetOtlpHttpLogRecordBuilder() const
  {
    return otlp_http_log_record_builder_;
  }
  void SetOtlpHttpLogRecordBuilder(const OtlpHttpLogRecordExporterBuilder *builder)
  {
    otlp_http_log_record_builder_ = builder;
  }

  const OtlpGrpcLogRecordExporterBuilder *GetOtlpGrpcLogRecordBuilder() const
  {
    return otlp_grpc_log_record_builder_;
  }
  void SetOtlpGrpcLogRecordBuilder(const OtlpGrpcLogRecordExporterBuilder *builder)
  {
    otlp_grpc_log_record_builder_ = builder;
  }

  const OtlpFileLogRecordExporterBuilder *GetOtlpFileLogRecordBuilder() const
  {
    return otlp_file_log_record_builder_;
  }
  void SetOtlpFileLogRecordBuilder(const OtlpFileLogRecordExporterBuilder *builder)
  {
    otlp_file_log_record_builder_ = builder;
  }

  const ConsoleLogRecordExporterBuilder *GetConsoleLogRecordBuilder() const
  {
    return console_log_record_builder_;
  }
  void SetConsoleLogRecordBuilder(const ConsoleLogRecordExporterBuilder *builder)
  {
    console_log_record_builder_ = builder;
  }

  /* Extension points */

  const TextMapPropagatorBuilder *GetTextMapPropagatorBuilder(const std::string &name) const;
  void AddTextMapPropagatorBuilder(const std::string &name, TextMapPropagatorBuilder *builder);

  const ExtensionSamplerBuilder *GetExtensionSamplerBuilder(const std::string &name) const;
  void AddExtensionSamplerBuilder(const std::string &name, ExtensionSamplerBuilder *builder);

  const ExtensionSpanExporterBuilder *GetExtensionSpanExporterBuilder(
      const std::string &name) const;
  void AddExtensionSpanExporterBuilder(const std::string &name,
                                       ExtensionSpanExporterBuilder *builder);

  const ExtensionSpanProcessorBuilder *GetExtensionSpanProcessorBuilder(
      const std::string &name) const;
  void AddExtensionSpanProcessorBuilder(const std::string &name,
                                        ExtensionSpanProcessorBuilder *builder);

  const ExtensionPushMetricExporterBuilder *GetExtensionPushMetricExporterBuilder(
      const std::string &name) const;
  void AddExtensionPushMetricExporterBuilder(const std::string &name,
                                             ExtensionPushMetricExporterBuilder *builder);

  const ExtensionPullMetricExporterBuilder *GetExtensionPullMetricExporterBuilder(
      const std::string &name) const;
  void AddExtensionPullMetricExporterBuilder(const std::string &name,
                                             ExtensionPullMetricExporterBuilder *builder);

  const ExtensionLogRecordExporterBuilder *GetExtensionLogRecordExporterBuilder(
      const std::string &name) const;
  void AddExtensionLogRecordExporterBuilder(const std::string &name,
                                            ExtensionLogRecordExporterBuilder *builder);

  const ExtensionLogRecordProcessorBuilder *GetExtensionLogRecordProcessorBuilder(
      const std::string &name) const;
  void AddExtensionLogRecordProcessorBuilder(const std::string &name,
                                             ExtensionLogRecordProcessorBuilder *builder);

private:
  const OtlpHttpSpanExporterBuilder *otlp_http_span_builder_{nullptr};
  const OtlpGrpcSpanExporterBuilder *otlp_grpc_span_builder_{nullptr};
  const OtlpFileSpanExporterBuilder *otlp_file_span_builder_{nullptr};
  const ConsoleSpanExporterBuilder *console_span_builder_{nullptr};
  const ZipkinSpanExporterBuilder *zipkin_span_builder_{nullptr};

  const OtlpHttpPushMetricExporterBuilder *otlp_http_push_metric_builder_{nullptr};
  const OtlpGrpcPushMetricExporterBuilder *otlp_grpc_push_metric_builder_{nullptr};
  const OtlpFilePushMetricExporterBuilder *otlp_file_push_metric_builder_{nullptr};
  const ConsolePushMetricExporterBuilder *console_metric_builder_{nullptr};
  const PrometheusPullMetricExporterBuilder *prometheus_metric_builder_{nullptr};

  const OtlpHttpLogRecordExporterBuilder *otlp_http_log_record_builder_{nullptr};
  const OtlpGrpcLogRecordExporterBuilder *otlp_grpc_log_record_builder_{nullptr};
  const OtlpFileLogRecordExporterBuilder *otlp_file_log_record_builder_{nullptr};
  const ConsoleLogRecordExporterBuilder *console_log_record_builder_{nullptr};

  std::map<std::string, TextMapPropagatorBuilder *> propagator_builders_;
  std::map<std::string, ExtensionSamplerBuilder *> sampler_builders_;
  std::map<std::string, ExtensionSpanExporterBuilder *> span_exporter_builders_;
  std::map<std::string, ExtensionSpanProcessorBuilder *> span_processor_builders_;
  std::map<std::string, ExtensionPushMetricExporterBuilder *> push_metric_exporter_builders_;
  std::map<std::string, ExtensionPullMetricExporterBuilder *> pull_metric_exporter_builders_;
  std::map<std::string, ExtensionLogRecordExporterBuilder *> log_record_exporter_builders_;
  std::map<std::string, ExtensionLogRecordProcessorBuilder *> log_record_processor_builders_;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
