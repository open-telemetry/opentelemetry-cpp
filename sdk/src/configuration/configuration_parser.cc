// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <stddef.h>
#include <fstream>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/configuration/aggregation_configuration.h"
#include "opentelemetry/sdk/configuration/always_off_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/always_on_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/attribute_limits_configuration.h"
#include "opentelemetry/sdk/configuration/attributes_configuration.h"
#include "opentelemetry/sdk/configuration/base2_exponential_bucket_histogram_aggregation_configuration.h"
#include "opentelemetry/sdk/configuration/batch_log_record_processor_configuration.h"
#include "opentelemetry/sdk/configuration/batch_span_processor_configuration.h"
#include "opentelemetry/sdk/configuration/configuration.h"
#include "opentelemetry/sdk/configuration/configuration_parser.h"
#include "opentelemetry/sdk/configuration/console_log_record_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/console_push_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/console_span_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/default_aggregation_configuration.h"
#include "opentelemetry/sdk/configuration/default_histogram_aggregation.h"
#include "opentelemetry/sdk/configuration/detectors_configuration.h"
#include "opentelemetry/sdk/configuration/document.h"
#include "opentelemetry/sdk/configuration/document_node.h"
#include "opentelemetry/sdk/configuration/drop_aggregation_configuration.h"
#include "opentelemetry/sdk/configuration/explicit_bucket_histogram_aggregation_configuration.h"
#include "opentelemetry/sdk/configuration/extension_log_record_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/extension_log_record_processor_configuration.h"
#include "opentelemetry/sdk/configuration/extension_pull_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/extension_push_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/extension_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/extension_span_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/extension_span_processor_configuration.h"
#include "opentelemetry/sdk/configuration/headers_configuration.h"
#include "opentelemetry/sdk/configuration/invalid_schema_exception.h"
#include "opentelemetry/sdk/configuration/jaeger_remote_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/last_value_aggregation_configuration.h"
#include "opentelemetry/sdk/configuration/log_record_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/log_record_limits_configuration.h"
#include "opentelemetry/sdk/configuration/log_record_processor_configuration.h"
#include "opentelemetry/sdk/configuration/logger_provider_configuration.h"
#include "opentelemetry/sdk/configuration/meter_provider_configuration.h"
#include "opentelemetry/sdk/configuration/metric_reader_configuration.h"
#include "opentelemetry/sdk/configuration/otlp_file_log_record_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/otlp_file_push_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/otlp_file_span_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/otlp_grpc_log_record_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/otlp_grpc_push_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/otlp_grpc_span_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/otlp_http_log_record_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/otlp_http_push_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/otlp_http_span_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/parent_based_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/periodic_metric_reader_configuration.h"
#include "opentelemetry/sdk/configuration/prometheus_pull_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/propagator_configuration.h"
#include "opentelemetry/sdk/configuration/pull_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/pull_metric_reader_configuration.h"
#include "opentelemetry/sdk/configuration/push_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/resource_configuration.h"
#include "opentelemetry/sdk/configuration/sampler_configuration.h"
#include "opentelemetry/sdk/configuration/simple_log_record_processor_configuration.h"
#include "opentelemetry/sdk/configuration/simple_span_processor_configuration.h"
#include "opentelemetry/sdk/configuration/span_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/span_limits_configuration.h"
#include "opentelemetry/sdk/configuration/span_processor_configuration.h"
#include "opentelemetry/sdk/configuration/string_array_configuration.h"
#include "opentelemetry/sdk/configuration/sum_aggregation_configuration.h"
#include "opentelemetry/sdk/configuration/trace_id_ratio_based_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/tracer_provider_configuration.h"
#include "opentelemetry/sdk/configuration/view_configuration.h"
#include "opentelemetry/sdk/configuration/view_selector_configuration.h"
#include "opentelemetry/sdk/configuration/view_stream_configuration.h"
#include "opentelemetry/sdk/configuration/zipkin_span_exporter_configuration.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

// FIXME: proper sizing
constexpr size_t MAX_SAMPLER_DEPTH = 10;

static std::unique_ptr<HeadersConfiguration> ParseHeadersConfiguration(
    const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<HeadersConfiguration> model(new HeadersConfiguration);
  std::unique_ptr<DocumentNode> kv_pair;
  std::unique_ptr<DocumentNode> name_child;
  std::unique_ptr<DocumentNode> value_child;
  std::string name;
  std::string value;

  for (auto it = node->begin(); it != node->end(); ++it)
  {
    kv_pair = *it;

    name_child  = kv_pair->GetRequiredChildNode("name");
    value_child = kv_pair->GetRequiredChildNode("value");

    name  = name_child->AsString();
    value = value_child->AsString();

    OTEL_INTERNAL_LOG_DEBUG("ParseHeadersConfiguration() name = " << name << ", value = " << value);
    std::pair<std::string, std::string> entry(name, value);
    model->kv_map.insert(entry);
  }

  return model;
}

static std::unique_ptr<AttributeLimitsConfiguration> ParseAttributeLimitsConfiguration(
    const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<AttributeLimitsConfiguration> model(new AttributeLimitsConfiguration);

  model->attribute_value_length_limit = node->GetInteger("attribute_value_length_limit", 4096);
  model->attribute_count_limit        = node->GetInteger("attribute_count_limit", 128);

  return model;
}

static std::unique_ptr<OtlpHttpLogRecordExporterConfiguration>
ParseOtlpHttpLogRecordExporterConfiguration(const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<OtlpHttpLogRecordExporterConfiguration> model(
      new OtlpHttpLogRecordExporterConfiguration);
  std::unique_ptr<DocumentNode> child;

  model->endpoint                = node->GetRequiredString("endpoint");
  model->certificate_file        = node->GetString("certificate_file", "");
  model->client_key_file         = node->GetString("client_key_file", "");
  model->client_certificate_file = node->GetString("client_certificate_file", "");

  child = node->GetChildNode("headers");
  if (child)
  {
    model->headers = ParseHeadersConfiguration(child);
  }

  model->headers_list = node->GetString("headers_list", "");
  model->compression  = node->GetString("compression", "");
  model->timeout      = node->GetInteger("timeout", 10000);
  // FIXME: encoding

  return model;
}

static std::unique_ptr<OtlpGrpcLogRecordExporterConfiguration>
ParseOtlpGrpcLogRecordExporterConfiguration(const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<OtlpGrpcLogRecordExporterConfiguration> model(
      new OtlpGrpcLogRecordExporterConfiguration);
  std::unique_ptr<DocumentNode> child;

  model->endpoint                = node->GetRequiredString("endpoint");
  model->certificate_file        = node->GetString("certificate_file", "");
  model->client_key_file         = node->GetString("client_key_file", "");
  model->client_certificate_file = node->GetString("client_certificate_file", "");

  child = node->GetChildNode("headers");
  if (child)
  {
    model->headers = ParseHeadersConfiguration(child);
  }

  model->headers_list = node->GetString("headers_list", "");
  model->compression  = node->GetString("compression", "");
  model->timeout      = node->GetInteger("timeout", 10000);
  model->insecure     = node->GetBoolean("insecure", false);

  return model;
}

static std::unique_ptr<OtlpFileLogRecordExporterConfiguration>
ParseOtlpFileLogRecordExporterConfiguration(const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<OtlpFileLogRecordExporterConfiguration> model(
      new OtlpFileLogRecordExporterConfiguration);
  std::unique_ptr<DocumentNode> child;

  model->output_stream = node->GetString("output_stream", "");

  return model;
}

static std::unique_ptr<ConsoleLogRecordExporterConfiguration>
ParseConsoleLogRecordExporterConfiguration(const std::unique_ptr<DocumentNode> & /* node */)
{
  std::unique_ptr<ConsoleLogRecordExporterConfiguration> model(
      new ConsoleLogRecordExporterConfiguration);

  return model;
}

static std::unique_ptr<ExtensionLogRecordExporterConfiguration>
ParseExtensionLogRecordExporterConfiguration(const std::string &name,
                                             std::unique_ptr<DocumentNode> node)
{
  std::unique_ptr<ExtensionLogRecordExporterConfiguration> model(
      new ExtensionLogRecordExporterConfiguration);
  model->name = name;
  model->node = std::move(node);
  return model;
}

static std::unique_ptr<LogRecordExporterConfiguration> ParseLogRecordExporterConfiguration(
    const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<LogRecordExporterConfiguration> model;

  std::string name;
  std::unique_ptr<DocumentNode> child;
  size_t count = 0;

  for (auto it = node->begin_properties(); it != node->end_properties(); ++it)
  {
    name  = it.Name();
    child = it.Value();
    count++;
  }

  if (count != 1)
  {
    OTEL_INTERNAL_LOG_ERROR("ParseLogRecordExporterConfiguration: count " << count);
    throw InvalidSchemaException("Illegal span exporter");
  }

  if (name == "otlp_http")
  {
    model = ParseOtlpHttpLogRecordExporterConfiguration(child);
  }
  else if (name == "otlp_grpc")
  {
    model = ParseOtlpGrpcLogRecordExporterConfiguration(child);
  }
  else if (name == "otlp_file/development")
  {
    model = ParseOtlpFileLogRecordExporterConfiguration(child);
  }
  else if (name == "console")
  {
    model = ParseConsoleLogRecordExporterConfiguration(child);
  }
  else
  {
    model = ParseExtensionLogRecordExporterConfiguration(name, std::move(child));
  }

  return model;
}

static std::unique_ptr<BatchLogRecordProcessorConfiguration>
ParseBatchLogRecordProcessorConfiguration(const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<BatchLogRecordProcessorConfiguration> model(
      new BatchLogRecordProcessorConfiguration);
  std::unique_ptr<DocumentNode> child;

  model->schedule_delay        = node->GetInteger("schedule_delay", 5000);
  model->export_timeout        = node->GetInteger("export_timeout", 30000);
  model->max_queue_size        = node->GetInteger("max_queue_size", 2048);
  model->max_export_batch_size = node->GetInteger("max_export_batch_size", 512);

  child           = node->GetRequiredChildNode("exporter");
  model->exporter = ParseLogRecordExporterConfiguration(child);

  return model;
}

static std::unique_ptr<SimpleLogRecordProcessorConfiguration>
ParseSimpleLogRecordProcessorConfiguration(const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<SimpleLogRecordProcessorConfiguration> model(
      new SimpleLogRecordProcessorConfiguration);
  std::unique_ptr<DocumentNode> child;

  child           = node->GetRequiredChildNode("exporter");
  model->exporter = ParseLogRecordExporterConfiguration(child);

  return model;
}

static std::unique_ptr<ExtensionLogRecordProcessorConfiguration>
ParseExtensionLogRecordProcessorConfiguration(const std::string &name,
                                              std::unique_ptr<DocumentNode> node)
{
  std::unique_ptr<ExtensionLogRecordProcessorConfiguration> model(
      new ExtensionLogRecordProcessorConfiguration);
  model->name = name;
  model->node = std::move(node);
  return model;
}

static std::unique_ptr<LogRecordProcessorConfiguration> ParseLogRecordProcessorConfiguration(
    const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<LogRecordProcessorConfiguration> model;

  std::string name;
  std::unique_ptr<DocumentNode> child;
  size_t count = 0;

  for (auto it = node->begin_properties(); it != node->end_properties(); ++it)
  {
    name  = it.Name();
    child = it.Value();
    count++;
  }

  if (count != 1)
  {
    OTEL_INTERNAL_LOG_ERROR("ParseLogRecordProcessorConfiguration: count " << count);
    throw InvalidSchemaException("Illegal log record processor");
  }

  if (name == "batch")
  {
    model = ParseBatchLogRecordProcessorConfiguration(child);
  }
  else if (name == "simple")
  {
    model = ParseSimpleLogRecordProcessorConfiguration(child);
  }
  else
  {
    model = ParseExtensionLogRecordProcessorConfiguration(name, std::move(child));
  }

  return model;
}

static std::unique_ptr<LogRecordLimitsConfiguration> ParseLogRecordLimitsConfiguration(
    const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<LogRecordLimitsConfiguration> model(new LogRecordLimitsConfiguration);

  model->attribute_value_length_limit = node->GetInteger("attribute_value_length_limit", 4096);
  model->attribute_count_limit        = node->GetInteger("attribute_count_limit", 128);

  return model;
}

static std::unique_ptr<LoggerProviderConfiguration> ParseLoggerProviderConfiguration(
    const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<LoggerProviderConfiguration> model(new LoggerProviderConfiguration);
  std::unique_ptr<DocumentNode> child;

  child = node->GetRequiredChildNode("processors");

  for (auto it = child->begin(); it != child->end(); ++it)
  {
    model->processors.push_back(ParseLogRecordProcessorConfiguration(*it));
  }

  size_t count = model->processors.size();
  if (count == 0)
  {
    OTEL_INTERNAL_LOG_ERROR("ParseLoggerProviderConfiguration: 0 processors ");
    throw InvalidSchemaException("Illegal processors");
  }

  child = node->GetChildNode("limits");
  if (child)
  {
    model->limits = ParseLogRecordLimitsConfiguration(child);
  }

  return model;
}

static enum_default_histogram_aggregation ParseDefaultHistogramAggregation(const std::string &name)
{
  if (name == "explicit_bucket_histogram")
  {
    return explicit_bucket_histogram;
  }

  if (name == "base2_exponential_bucket_histogram")
  {
    return base2_exponential_bucket_histogram;
  }

  OTEL_INTERNAL_LOG_ERROR("ParseDefaultHistogramAggregation: name = " << name);
  throw InvalidSchemaException("Illegal default_histogram_aggregation");
}

static enum_temporality_preference ParseTemporalityPreference(const std::string &name)
{
  if (name == "cumulative")
  {
    return cumulative;
  }

  if (name == "delta")
  {
    return delta;
  }

  if (name == "low_memory")
  {
    return low_memory;
  }

  OTEL_INTERNAL_LOG_ERROR("ParseTemporalityPreference: name = " << name);
  throw InvalidSchemaException("Illegal temporality preference");
}

static std::unique_ptr<OtlpHttpPushMetricExporterConfiguration>
ParseOtlpHttpPushMetricExporterConfiguration(const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<OtlpHttpPushMetricExporterConfiguration> model(
      new OtlpHttpPushMetricExporterConfiguration);
  std::unique_ptr<DocumentNode> child;

  model->endpoint                = node->GetRequiredString("endpoint");
  model->certificate_file        = node->GetString("certificate_file", "");
  model->client_key_file         = node->GetString("client_key_file", "");
  model->client_certificate_file = node->GetString("client_certificate_file", "");

  child = node->GetChildNode("headers");
  if (child)
  {
    model->headers = ParseHeadersConfiguration(child);
  }

  model->headers_list = node->GetString("headers_list", "");
  model->compression  = node->GetString("compression", "");
  model->timeout      = node->GetInteger("timeout", 10000);

  std::string temporality_preference = node->GetString("temporality_preference", "");
  model->temporality_preference      = ParseTemporalityPreference(temporality_preference);

  std::string default_histogram_aggregation = node->GetString("default_histogram_aggregation", "");
  model->default_histogram_aggregation =
      ParseDefaultHistogramAggregation(default_histogram_aggregation);

  return model;
}

static std::unique_ptr<OtlpGrpcPushMetricExporterConfiguration>
ParseOtlpGrpcPushMetricExporterConfiguration(const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<OtlpGrpcPushMetricExporterConfiguration> model(
      new OtlpGrpcPushMetricExporterConfiguration);
  std::unique_ptr<DocumentNode> child;

  model->endpoint                = node->GetRequiredString("endpoint");
  model->certificate_file        = node->GetString("certificate_file", "");
  model->client_key_file         = node->GetString("client_key_file", "");
  model->client_certificate_file = node->GetString("client_certificate_file", "");

  child = node->GetChildNode("headers");
  if (child)
  {
    model->headers = ParseHeadersConfiguration(child);
  }

  model->headers_list = node->GetString("headers_list", "");
  model->compression  = node->GetString("compression", "");
  model->timeout      = node->GetInteger("timeout", 10000);

  std::string temporality_preference = node->GetString("temporality_preference", "");
  model->temporality_preference      = ParseTemporalityPreference(temporality_preference);

  std::string default_histogram_aggregation = node->GetString("default_histogram_aggregation", "");
  model->default_histogram_aggregation =
      ParseDefaultHistogramAggregation(default_histogram_aggregation);

  model->insecure = node->GetBoolean("insecure", false);

  return model;
}

static std::unique_ptr<OtlpFilePushMetricExporterConfiguration>
ParseOtlpFilePushMetricExporterConfiguration(const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<OtlpFilePushMetricExporterConfiguration> model(
      new OtlpFilePushMetricExporterConfiguration);
  std::unique_ptr<DocumentNode> child;

  model->output_stream = node->GetString("output_stream", "");

  std::string temporality_preference = node->GetString("temporality_preference", "");
  model->temporality_preference      = ParseTemporalityPreference(temporality_preference);

  std::string default_histogram_aggregation = node->GetString("default_histogram_aggregation", "");
  model->default_histogram_aggregation =
      ParseDefaultHistogramAggregation(default_histogram_aggregation);

  return model;
}

static std::unique_ptr<ConsolePushMetricExporterConfiguration>
ParseConsolePushMetricExporterConfiguration(const std::unique_ptr<DocumentNode> & /* node */)
{
  std::unique_ptr<ConsolePushMetricExporterConfiguration> model(
      new ConsolePushMetricExporterConfiguration);

  return model;
}

static std::unique_ptr<PrometheusPullMetricExporterConfiguration>
ParsePrometheusPullMetricExporterConfiguration(const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<PrometheusPullMetricExporterConfiguration> model(
      new PrometheusPullMetricExporterConfiguration);

  model->host                = node->GetString("host", "localhost");
  model->port                = node->GetInteger("port", 9464);
  model->without_units       = node->GetBoolean("without_units", false);
  model->without_type_suffix = node->GetBoolean("without_type_suffix", false);
  model->without_scope_info  = node->GetBoolean("without_scope_info", false);

  return model;
}

static std::unique_ptr<ExtensionPushMetricExporterConfiguration>
ParsePushMetricExporterExtensionConfiguration(const std::string &name,
                                              std::unique_ptr<DocumentNode> node)
{
  std::unique_ptr<ExtensionPushMetricExporterConfiguration> model(
      new ExtensionPushMetricExporterConfiguration);
  model->name = name;
  model->node = std::move(node);
  return model;
}

static std::unique_ptr<ExtensionPullMetricExporterConfiguration>
ParsePullMetricExporterExtensionConfiguration(const std::string &name,
                                              std::unique_ptr<DocumentNode> node)
{
  std::unique_ptr<ExtensionPullMetricExporterConfiguration> model(
      new ExtensionPullMetricExporterConfiguration);
  model->name = name;
  model->node = std::move(node);
  return model;
}

static std::unique_ptr<PushMetricExporterConfiguration> ParsePushMetricExporterConfiguration(
    const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<PushMetricExporterConfiguration> model;

  std::string name;
  std::unique_ptr<DocumentNode> child;
  size_t count = 0;

  for (auto it = node->begin_properties(); it != node->end_properties(); ++it)
  {
    name  = it.Name();
    child = it.Value();
    count++;
  }

  if (count != 1)
  {
    OTEL_INTERNAL_LOG_ERROR("ParsePushMetricExporterConfiguration: count " << count);
    throw InvalidSchemaException("Illegal metric exporter");
  }

  if (name == "otlp_http")
  {
    model = ParseOtlpHttpPushMetricExporterConfiguration(child);
  }
  else if (name == "otlp_grpc")
  {
    model = ParseOtlpGrpcPushMetricExporterConfiguration(child);
  }
  else if (name == "otlp_file/development")
  {
    model = ParseOtlpFilePushMetricExporterConfiguration(child);
  }
  else if (name == "console")
  {
    model = ParseConsolePushMetricExporterConfiguration(child);
  }
  else
  {
    model = ParsePushMetricExporterExtensionConfiguration(name, std::move(child));
  }

  return model;
}

static std::unique_ptr<PullMetricExporterConfiguration> ParsePullMetricExporterConfiguration(
    const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<PullMetricExporterConfiguration> model;

  std::string name;
  std::unique_ptr<DocumentNode> child;
  size_t count = 0;

  for (auto it = node->begin_properties(); it != node->end_properties(); ++it)
  {
    name  = it.Name();
    child = it.Value();
    count++;
  }

  if (count != 1)
  {
    OTEL_INTERNAL_LOG_ERROR("ParsePullMetricExporterConfiguration: count " << count);
    throw InvalidSchemaException("Illegal metric exporter");
  }

  if (name == "prometheus")
  {
    model = ParsePrometheusPullMetricExporterConfiguration(child);
  }
  else
  {
    model = ParsePullMetricExporterExtensionConfiguration(name, std::move(child));
  }

  return model;
}

static std::unique_ptr<PeriodicMetricReaderConfiguration> ParsePeriodicMetricReaderConfiguration(
    const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<PeriodicMetricReaderConfiguration> model(new PeriodicMetricReaderConfiguration);
  std::unique_ptr<DocumentNode> child;

  model->interval = node->GetInteger("interval", 5000);
  model->timeout  = node->GetInteger("timeout", 30000);

  child           = node->GetRequiredChildNode("exporter");
  model->exporter = ParsePushMetricExporterConfiguration(child);

  return model;
}

static std::unique_ptr<PullMetricReaderConfiguration> ParsePullMetricReaderConfiguration(
    const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<PullMetricReaderConfiguration> model(new PullMetricReaderConfiguration);
  std::unique_ptr<DocumentNode> child;

  child           = node->GetRequiredChildNode("exporter");
  model->exporter = ParsePullMetricExporterConfiguration(child);

  return model;
}

static std::unique_ptr<MetricReaderConfiguration> ParseMetricReaderConfiguration(
    const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<MetricReaderConfiguration> model;

  std::string name;
  std::unique_ptr<DocumentNode> child;
  size_t count = 0;

  for (auto it = node->begin_properties(); it != node->end_properties(); ++it)
  {
    name  = it.Name();
    child = it.Value();
    count++;
  }

  if (count != 1)
  {
    OTEL_INTERNAL_LOG_ERROR("ParseMetricReaderConfiguration: count " << count);
    throw InvalidSchemaException("Illegal metric reader");
  }

  if (name == "periodic")
  {
    model = ParsePeriodicMetricReaderConfiguration(child);
  }
  else if (name == "pull")
  {
    model = ParsePullMetricReaderConfiguration(child);
  }
  else
  {
    OTEL_INTERNAL_LOG_ERROR("ParseMetricReaderConfiguration: name " << name);
    throw InvalidSchemaException("Illegal metric reader");
  }

  return model;
}

static enum_instrument_type ParseInstrumentType(const std::string &name)
{
  if (name == "counter")
  {
    return counter;
  }

  if (name == "histogram")
  {
    return histogram;
  }

  if (name == "observable_counter")
  {
    return observable_counter;
  }

  if (name == "observable_gauge")
  {
    return observable_gauge;
  }

  if (name == "observable_up_down_counter")
  {
    return observable_up_down_counter;
  }

  if (name == "up_down_counter")
  {
    return up_down_counter;
  }

  OTEL_INTERNAL_LOG_ERROR("ParseInstrumentType: name = " << name);
  throw InvalidSchemaException("Illegal instrument_type");
}

static std::unique_ptr<ViewSelectorConfiguration> ParseViewSelectorConfiguration(
    const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<ViewSelectorConfiguration> model(new ViewSelectorConfiguration);

  model->instrument_name = node->GetString("instrument_name", "");

  std::string instrument_type = node->GetString("instrument_type", "");
  model->instrument_type      = ParseInstrumentType(instrument_type);

  model->unit             = node->GetString("unit", "");
  model->meter_name       = node->GetString("meter_name", "");
  model->meter_version    = node->GetString("meter_version", "");
  model->meter_schema_url = node->GetString("meter_schema_url", "");

  return model;
}

static std::unique_ptr<DefaultAggregationConfiguration> ParseDefaultAggregationConfiguration(
    const std::unique_ptr<DocumentNode> & /* node */)
{
  std::unique_ptr<DefaultAggregationConfiguration> model(new DefaultAggregationConfiguration);

  return model;
}

static std::unique_ptr<DropAggregationConfiguration> ParseDropAggregationConfiguration(
    const std::unique_ptr<DocumentNode> & /* node */)
{
  std::unique_ptr<DropAggregationConfiguration> model(new DropAggregationConfiguration);

  return model;
}

static std::unique_ptr<ExplicitBucketHistogramAggregationConfiguration>
ParseExplicitBucketHistogramAggregationConfiguration(const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<ExplicitBucketHistogramAggregationConfiguration> model(
      new ExplicitBucketHistogramAggregationConfiguration);
  std::unique_ptr<DocumentNode> child;

  child = node->GetChildNode("boundaries");

  if (child)
  {
    for (auto it = child->begin(); it != child->end(); ++it)
    {
      std::unique_ptr<DocumentNode> attribute_key(*it);

      double boundary = attribute_key->AsDouble();

      model->boundaries.push_back(boundary);
    }
  }

  model->record_min_max = node->GetBoolean("record_min_max", true);

  return model;
}

static std::unique_ptr<Base2ExponentialBucketHistogramAggregationConfiguration>
ParseBase2ExponentialBucketHistogramAggregationConfiguration(
    const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<Base2ExponentialBucketHistogramAggregationConfiguration> model(
      new Base2ExponentialBucketHistogramAggregationConfiguration);

  model->max_scale      = node->GetInteger("max_scale", 0);  // FIXME: default ?
  model->max_size       = node->GetInteger("max_size", 0);   // FIXME: default ?
  model->record_min_max = node->GetBoolean("record_min_max", true);

  return model;
}

static std::unique_ptr<LastValueAggregationConfiguration> ParseLastValueAggregationConfiguration(
    const std::unique_ptr<DocumentNode> & /* node */)
{
  std::unique_ptr<LastValueAggregationConfiguration> model(new LastValueAggregationConfiguration);

  return model;
}

static std::unique_ptr<SumAggregationConfiguration> ParseSumAggregationConfiguration(
    const std::unique_ptr<DocumentNode> & /* node */)
{
  std::unique_ptr<SumAggregationConfiguration> model(new SumAggregationConfiguration);

  return model;
}

static std::unique_ptr<AggregationConfiguration> ParseAggregationConfiguration(
    const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<AggregationConfiguration> model;
  std::unique_ptr<DocumentNode> child;

  size_t count = node->num_children();

  if (count != 1)
  {
    OTEL_INTERNAL_LOG_ERROR("ParseAggregationConfiguration: count " << count);
    throw InvalidSchemaException("Illegal aggregation");
  }

  child            = node->GetChild(0);
  std::string name = child->Key();

  if (name == "default")
  {
    model = ParseDefaultAggregationConfiguration(child);
  }
  else if (name == "drop")
  {
    model = ParseDropAggregationConfiguration(child);
  }
  else if (name == "explicit_bucket_histogram")
  {
    model = ParseExplicitBucketHistogramAggregationConfiguration(child);
  }
  else if (name == "base2_exponential_bucket_histogram")
  {
    model = ParseBase2ExponentialBucketHistogramAggregationConfiguration(child);
  }
  else if (name == "last_value")
  {
    model = ParseLastValueAggregationConfiguration(child);
  }
  else if (name == "sum")
  {
    model = ParseSumAggregationConfiguration(child);
  }
  else
  {
    OTEL_INTERNAL_LOG_ERROR("ParseAggregationConfiguration: name " << name);
    throw InvalidSchemaException("Illegal aggregation");
  }

  return model;
}

static std::unique_ptr<ViewStreamConfiguration> ParseViewStreamConfiguration(
    const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<ViewStreamConfiguration> model(new ViewStreamConfiguration);
  std::unique_ptr<DocumentNode> child;

  model->name        = node->GetString("name", "");
  model->description = node->GetString("description", "");

  child = node->GetChildNode("aggregation");
  if (child)
  {
    model->aggregation = ParseAggregationConfiguration(child);
  }

  child = node->GetChildNode("attribute_keys");

  if (child)
  {
    OTEL_INTERNAL_LOG_ERROR("ParseViewStreamConfiguration: FIXME");

    // Schema has changed
#ifdef NEVER
    for (auto it = child->begin(); it != child->end(); ++it)
    {
      std::unique_ptr<DocumentNode> attribute_key(*it);

      std::string name = attribute_key->AsString();

      model->attribute_keys.push_back(name);
    }
#endif
  }

  return model;
}

static std::unique_ptr<ViewConfiguration> ParseViewConfiguration(
    const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<ViewConfiguration> model(new ViewConfiguration);
  std::unique_ptr<DocumentNode> child;

  child           = node->GetRequiredChildNode("selector");
  model->selector = ParseViewSelectorConfiguration(child);

  child         = node->GetRequiredChildNode("stream");
  model->stream = ParseViewStreamConfiguration(child);

  return model;
}

static std::unique_ptr<MeterProviderConfiguration> ParseMeterProviderConfiguration(
    const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<MeterProviderConfiguration> model(new MeterProviderConfiguration);
  std::unique_ptr<DocumentNode> child;

  child = node->GetRequiredChildNode("readers");

  for (auto it = child->begin(); it != child->end(); ++it)
  {
    model->readers.push_back(ParseMetricReaderConfiguration(*it));
  }

  child = node->GetRequiredChildNode("views");

  for (auto it = child->begin(); it != child->end(); ++it)
  {
    model->views.push_back(ParseViewConfiguration(*it));
  }

  return model;
}

static std::unique_ptr<PropagatorConfiguration> ParsePropagatorConfiguration(
    const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<PropagatorConfiguration> model(new PropagatorConfiguration);

  std::unique_ptr<DocumentNode> child;
  child = node->GetRequiredChildNode("composite");

  for (auto it = child->begin(); it != child->end(); ++it)
  {
    std::unique_ptr<DocumentNode> element(*it);

    std::string name = element->AsString();

    model->composite.push_back(name);
  }

  return model;
}

static std::unique_ptr<SpanLimitsConfiguration> ParseSpanLimitsConfiguration(
    const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<SpanLimitsConfiguration> model(new SpanLimitsConfiguration);

  model->attribute_value_length_limit = node->GetInteger("attribute_value_length_limit", 4096);
  model->attribute_count_limit        = node->GetInteger("attribute_count_limit", 128);
  model->event_count_limit            = node->GetInteger("event_count_limit", 128);
  model->link_count_limit             = node->GetInteger("link_count_limit", 128);
  model->event_attribute_count_limit  = node->GetInteger("event_attribute_count_limit", 128);
  model->link_attribute_count_limit   = node->GetInteger("link_attribute_count_limit", 128);

  return model;
}

static std::unique_ptr<SamplerConfiguration> ParseSamplerConfiguration(
    const std::unique_ptr<DocumentNode> &node,
    size_t depth);

static std::unique_ptr<AlwaysOffSamplerConfiguration> ParseAlwaysOffSamplerConfiguration(
    const std::unique_ptr<DocumentNode> & /* node */,
    size_t /* depth */)
{
  std::unique_ptr<AlwaysOffSamplerConfiguration> model(new AlwaysOffSamplerConfiguration);

  return model;
}

static std::unique_ptr<AlwaysOnSamplerConfiguration> ParseAlwaysOnSamplerConfiguration(
    const std::unique_ptr<DocumentNode> & /* node */,
    size_t /* depth */)
{
  std::unique_ptr<AlwaysOnSamplerConfiguration> model(new AlwaysOnSamplerConfiguration);

  return model;
}

// NOLINTBEGIN(misc-no-recursion)
static std::unique_ptr<JaegerRemoteSamplerConfiguration> ParseJaegerRemoteSamplerConfiguration(
    const std::unique_ptr<DocumentNode> &node,
    size_t depth)
{
  std::unique_ptr<JaegerRemoteSamplerConfiguration> model(new JaegerRemoteSamplerConfiguration);
  std::unique_ptr<DocumentNode> child;

  // Unclear if endpoint and interval are required/optional
  OTEL_INTERNAL_LOG_ERROR("JaegerRemoteSamplerConfiguration: FIXME");

  model->endpoint = node->GetString("endpoint", "FIXME");
  model->interval = node->GetInteger("interval", 0);

  child = node->GetChildNode("initial_sampler");
  if (child)
  {
    model->initial_sampler = ParseSamplerConfiguration(child, depth + 1);
  }

  return model;
}
// NOLINTEND(misc-no-recursion)

// NOLINTBEGIN(misc-no-recursion)
static std::unique_ptr<ParentBasedSamplerConfiguration> ParseParentBasedSamplerConfiguration(
    const std::unique_ptr<DocumentNode> &node,
    size_t depth)
{
  std::unique_ptr<ParentBasedSamplerConfiguration> model(new ParentBasedSamplerConfiguration);
  std::unique_ptr<DocumentNode> child;

  child = node->GetChildNode("root");
  if (child)
  {
    model->root = ParseSamplerConfiguration(child, depth + 1);
  }

  child = node->GetChildNode("remote_parent_sampled");
  if (child)
  {
    model->remote_parent_sampled = ParseSamplerConfiguration(child, depth + 1);
  }

  child = node->GetChildNode("remote_parent_not_sampled");
  if (child)
  {
    model->remote_parent_not_sampled = ParseSamplerConfiguration(child, depth + 1);
  }

  child = node->GetChildNode("local_parent_sampled");
  if (child)
  {
    model->local_parent_sampled = ParseSamplerConfiguration(child, depth + 1);
  }

  child = node->GetChildNode("local_parent_not_sampled");
  if (child)
  {
    model->local_parent_not_sampled = ParseSamplerConfiguration(child, depth + 1);
  }

  return model;
}
// NOLINTEND(misc-no-recursion)

static std::unique_ptr<TraceIdRatioBasedSamplerConfiguration>
ParseTraceIdRatioBasedSamplerConfiguration(const std::unique_ptr<DocumentNode> &node,
                                           size_t /* depth */)
{
  std::unique_ptr<TraceIdRatioBasedSamplerConfiguration> model(
      new TraceIdRatioBasedSamplerConfiguration);
  std::unique_ptr<DocumentNode> child;

  model->ratio = node->GetDouble("ratio", 0);

  return model;
}

static std::unique_ptr<ExtensionSamplerConfiguration> ParseSamplerExtensionConfiguration(
    const std::string &name,
    std::unique_ptr<DocumentNode> node,
    size_t depth)
{
  std::unique_ptr<ExtensionSamplerConfiguration> model(new ExtensionSamplerConfiguration);
  model->name  = name;
  model->node  = std::move(node);
  model->depth = depth;
  return model;
}

// NOLINTBEGIN(misc-no-recursion)
static std::unique_ptr<SamplerConfiguration> ParseSamplerConfiguration(
    const std::unique_ptr<DocumentNode> &node,
    size_t depth)
{
  /*
   * ParseSamplerConfiguration() is recursive,
   * enforce a limit to prevent attacks from yaml.
   */
  if (depth >= MAX_SAMPLER_DEPTH)
  {
    OTEL_INTERNAL_LOG_ERROR("ParseSamplerConfiguration: depth " << depth);
    throw InvalidSchemaException("Samplers nested too deeply");
  }

  std::unique_ptr<SamplerConfiguration> model;

  std::string name;
  std::unique_ptr<DocumentNode> child;
  size_t count = 0;

  for (auto it = node->begin_properties(); it != node->end_properties(); ++it)
  {
    name  = it.Name();
    child = it.Value();
    count++;
  }

  if (count != 1)
  {
    OTEL_INTERNAL_LOG_ERROR("ParseSamplerConfiguration: count " << count);
    throw InvalidSchemaException("Illegal sampler");
  }

  if (name == "always_off")
  {
    model = ParseAlwaysOffSamplerConfiguration(child, depth);
  }
  else if (name == "always_on")
  {
    model = ParseAlwaysOnSamplerConfiguration(child, depth);
  }
  else if (name == "jaeger_remote")
  {
    model = ParseJaegerRemoteSamplerConfiguration(child, depth);
  }
  else if (name == "parent_based")
  {
    model = ParseParentBasedSamplerConfiguration(child, depth);
  }
  else if (name == "trace_id_ratio_based")
  {
    model = ParseTraceIdRatioBasedSamplerConfiguration(child, depth);
  }
  else
  {
    model = ParseSamplerExtensionConfiguration(name, std::move(child), depth);
  }

  return model;
}
// NOLINTEND(misc-no-recursion)

static std::unique_ptr<OtlpHttpSpanExporterConfiguration> ParseOtlpHttpSpanExporterConfiguration(
    const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<OtlpHttpSpanExporterConfiguration> model(new OtlpHttpSpanExporterConfiguration);
  std::unique_ptr<DocumentNode> child;

  model->endpoint                = node->GetRequiredString("endpoint");
  model->certificate_file        = node->GetString("certificate_file", "");
  model->client_key_file         = node->GetString("client_key_file", "");
  model->client_certificate_file = node->GetString("client_certificate_file", "");

  child = node->GetChildNode("headers");
  if (child)
  {
    model->headers = ParseHeadersConfiguration(child);
  }

  model->headers_list = node->GetString("headers_list", "");
  model->compression  = node->GetString("compression", "");
  model->timeout      = node->GetInteger("timeout", 10000);

  return model;
}

static std::unique_ptr<OtlpGrpcSpanExporterConfiguration> ParseOtlpGrpcSpanExporterConfiguration(
    const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<OtlpGrpcSpanExporterConfiguration> model(new OtlpGrpcSpanExporterConfiguration);
  std::unique_ptr<DocumentNode> child;

  model->endpoint                = node->GetRequiredString("endpoint");
  model->certificate_file        = node->GetString("certificate_file", "");
  model->client_key_file         = node->GetString("client_key_file", "");
  model->client_certificate_file = node->GetString("client_certificate_file", "");

  child = node->GetChildNode("headers");
  if (child)
  {
    model->headers = ParseHeadersConfiguration(child);
  }

  model->headers_list = node->GetString("headers_list", "");
  model->compression  = node->GetString("compression", "");
  model->timeout      = node->GetInteger("timeout", 10000);
  model->insecure     = node->GetBoolean("insecure", false);

  return model;
}

static std::unique_ptr<OtlpFileSpanExporterConfiguration> ParseOtlpFileSpanExporterConfiguration(
    const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<OtlpFileSpanExporterConfiguration> model(new OtlpFileSpanExporterConfiguration);
  std::unique_ptr<DocumentNode> child;

  model->output_stream = node->GetString("output_stream", "");

  return model;
}

static std::unique_ptr<ConsoleSpanExporterConfiguration> ParseConsoleSpanExporterConfiguration(
    const std::unique_ptr<DocumentNode> & /* node */)
{
  std::unique_ptr<ConsoleSpanExporterConfiguration> model(new ConsoleSpanExporterConfiguration);

  return model;
}

static std::unique_ptr<ZipkinSpanExporterConfiguration> ParseZipkinSpanExporterConfiguration(
    const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<ZipkinSpanExporterConfiguration> model(new ZipkinSpanExporterConfiguration);

  model->endpoint = node->GetRequiredString("endpoint");
  model->timeout  = node->GetInteger("timeout", 10000);

  return model;
}

static std::unique_ptr<ExtensionSpanExporterConfiguration> ParseExtensionSpanExporterConfiguration(
    const std::string &name,
    std::unique_ptr<DocumentNode> node)
{
  std::unique_ptr<ExtensionSpanExporterConfiguration> model(new ExtensionSpanExporterConfiguration);
  model->name = name;
  model->node = std::move(node);
  return model;
}

static std::unique_ptr<SpanExporterConfiguration> ParseSpanExporterConfiguration(
    const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<SpanExporterConfiguration> model;

  std::string name;
  std::unique_ptr<DocumentNode> child;
  size_t count = 0;

  for (auto it = node->begin_properties(); it != node->end_properties(); ++it)
  {
    name  = it.Name();
    child = it.Value();
    count++;
  }

  if (count != 1)
  {
    OTEL_INTERNAL_LOG_ERROR("ParseSpanExporterConfiguration: count " << count);
    throw InvalidSchemaException("Illegal span exporter");
  }

  if (name == "otlp_http")
  {
    model = ParseOtlpHttpSpanExporterConfiguration(child);
  }
  else if (name == "otlp_grpc")
  {
    model = ParseOtlpGrpcSpanExporterConfiguration(child);
  }
  else if (name == "otlp_file/development")
  {
    model = ParseOtlpFileSpanExporterConfiguration(child);
  }
  else if (name == "console")
  {
    model = ParseConsoleSpanExporterConfiguration(child);
  }
  else if (name == "zipkin")
  {
    model = ParseZipkinSpanExporterConfiguration(child);
  }
  else
  {
    model = ParseExtensionSpanExporterConfiguration(name, std::move(child));
  }

  return model;
}

static std::unique_ptr<BatchSpanProcessorConfiguration> ParseBatchSpanProcessorConfiguration(
    const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<BatchSpanProcessorConfiguration> model(new BatchSpanProcessorConfiguration);
  std::unique_ptr<DocumentNode> child;

  model->schedule_delay        = node->GetInteger("schedule_delay", 5000);
  model->export_timeout        = node->GetInteger("export_timeout", 30000);
  model->max_queue_size        = node->GetInteger("max_queue_size", 2048);
  model->max_export_batch_size = node->GetInteger("max_export_batch_size", 512);

  child           = node->GetRequiredChildNode("exporter");
  model->exporter = ParseSpanExporterConfiguration(child);

  return model;
}

static std::unique_ptr<SimpleSpanProcessorConfiguration> ParseSimpleSpanProcessorConfiguration(
    const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<SimpleSpanProcessorConfiguration> model(new SimpleSpanProcessorConfiguration);
  std::unique_ptr<DocumentNode> child;

  child           = node->GetRequiredChildNode("exporter");
  model->exporter = ParseSpanExporterConfiguration(child);

  return model;
}

static std::unique_ptr<ExtensionSpanProcessorConfiguration>
ParseExtensionSpanProcessorConfiguration(const std::string &name,
                                         std::unique_ptr<DocumentNode> node)
{
  std::unique_ptr<ExtensionSpanProcessorConfiguration> model(
      new ExtensionSpanProcessorConfiguration);
  model->name = name;
  model->node = std::move(node);
  return model;
}

static std::unique_ptr<SpanProcessorConfiguration> ParseSpanProcessorConfiguration(
    const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<SpanProcessorConfiguration> model;

  std::string name;
  std::unique_ptr<DocumentNode> child;
  size_t count = 0;

  for (auto it = node->begin_properties(); it != node->end_properties(); ++it)
  {
    name  = it.Name();
    child = it.Value();
    count++;
  }

  if (count != 1)
  {
    OTEL_INTERNAL_LOG_ERROR("ParseSpanProcessorConfiguration: count " << count);
    throw InvalidSchemaException("Illegal span processor");
  }

  if (name == "batch")
  {
    model = ParseBatchSpanProcessorConfiguration(child);
  }
  else if (name == "simple")
  {
    model = ParseSimpleSpanProcessorConfiguration(child);
  }
  else
  {
    model = ParseExtensionSpanProcessorConfiguration(name, std::move(child));
  }

  return model;
}

static std::unique_ptr<TracerProviderConfiguration> ParseTracerProviderConfiguration(
    const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<TracerProviderConfiguration> model(new TracerProviderConfiguration);
  std::unique_ptr<DocumentNode> child;

  child = node->GetRequiredChildNode("processors");

  for (auto it = child->begin(); it != child->end(); ++it)
  {
    model->processors.push_back(ParseSpanProcessorConfiguration(*it));
  }

  size_t count = model->processors.size();
  if (count == 0)
  {
    OTEL_INTERNAL_LOG_ERROR("ParseTracerProviderConfiguration: 0 processors ");
    throw InvalidSchemaException("Illegal processors");
  }

  child = node->GetChildNode("limits");
  if (child)
  {
    model->limits = ParseSpanLimitsConfiguration(child);
  }

  child = node->GetChildNode("sampler");
  if (child)
  {
    model->sampler = ParseSamplerConfiguration(child, 0);
  }

  return model;
}

static std::unique_ptr<AttributesConfiguration> ParseAttributesConfiguration(
    const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<AttributesConfiguration> model(new AttributesConfiguration);
  std::unique_ptr<DocumentNode> child;

  OTEL_INTERNAL_LOG_ERROR("ParseAttributesConfiguration: FIXME");

  std::unique_ptr<DocumentNode> attribute_name_value;
  std::unique_ptr<DocumentNode> name_child;
  std::unique_ptr<DocumentNode> value_child;
  std::unique_ptr<DocumentNode> type_child;
  std::string name;
  std::string type;

  for (auto it = node->begin(); it != node->end(); ++it)
  {
    attribute_name_value = *it;

    name_child  = attribute_name_value->GetRequiredChildNode("name");
    value_child = attribute_name_value->GetRequiredChildNode("value");
    type_child  = attribute_name_value->GetChildNode("type");

    name = name_child->AsString();
    if (type_child)
    {
      type = type_child->AsString();
    }
    else
    {
      type = "string";
    }

    if (type == "string")
    {
      std::string value;
      value = value_child->AsString();
      OTEL_INTERNAL_LOG_DEBUG("ParseAttributesConfiguration() name = " << name
                                                                       << ", value = " << value);
      std::pair<std::string, std::string> entry(name, value);
      model->kv_map.insert(entry);
    }
    else if (type == "bool")
    {
      OTEL_INTERNAL_LOG_ERROR("ParseAttributesConfiguration: FIXME: bool");
    }
    else if (type == "int")
    {
      OTEL_INTERNAL_LOG_ERROR("ParseAttributesConfiguration: FIXME: int");
    }
    else if (type == "double")
    {
      OTEL_INTERNAL_LOG_ERROR("ParseAttributesConfiguration: FIXME: double");
    }
    else if (type == "string_array")
    {
      OTEL_INTERNAL_LOG_ERROR("ParseAttributesConfiguration: FIXME: string_array");
    }
    else if (type == "bool_array")
    {
      OTEL_INTERNAL_LOG_ERROR("ParseAttributesConfiguration: FIXME: bool_array");
    }
    else if (type == "int_array")
    {
      OTEL_INTERNAL_LOG_ERROR("ParseAttributesConfiguration: FIXME: int_array");
    }
    else if (type == "double_array")
    {
      OTEL_INTERNAL_LOG_ERROR("ParseAttributesConfiguration: FIXME: double_array");
    }
    else
    {
      OTEL_INTERNAL_LOG_ERROR("ParseAttributesConfiguration: unknown type " << type);
      throw InvalidSchemaException("Illegal attribute type");
    }
  }

  return model;
}

static std::unique_ptr<StringArrayConfiguration> ParseStringArrayConfiguration(
    const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<StringArrayConfiguration> model(new StringArrayConfiguration);

  for (auto it = node->begin(); it != node->end(); ++it)
  {
    std::unique_ptr<DocumentNode> child(*it);

    std::string name = child->AsString();

    model->string_array.push_back(name);
  }

  return model;
}

static std::unique_ptr<DetectorsConfiguration> ParseDetectorsConfiguration(
    const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<DetectorsConfiguration> model(new DetectorsConfiguration);
  std::unique_ptr<DocumentNode> child;

  child = node->GetChildNode("included");
  if (child)
  {
    model->included = ParseStringArrayConfiguration(child);
  }

  child = node->GetChildNode("excluded");
  if (child)
  {
    model->excluded = ParseStringArrayConfiguration(child);
  }

  return model;
}

static std::unique_ptr<ResourceConfiguration> ParseResourceConfiguration(
    const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<ResourceConfiguration> model(new ResourceConfiguration);
  std::unique_ptr<DocumentNode> child;

  model->schema_url      = node->GetString("schema_url", "");
  model->attributes_list = node->GetString("attributes_list", "");

  child = node->GetChildNode("attributes");
  if (child)
  {
    model->attributes = ParseAttributesConfiguration(child);
  }

  child = node->GetChildNode("detectors");
  if (child)
  {
    model->detectors = ParseDetectorsConfiguration(child);
  }

  return model;
}

std::unique_ptr<Configuration> ConfigurationParser::Parse(std::unique_ptr<Document> doc)
{
  std::unique_ptr<DocumentNode> node = doc->GetRootNode();
  std::unique_ptr<Configuration> model(new Configuration(std::move(doc)));

  model->file_format = node->GetRequiredString("file_format");
  model->disabled    = node->GetBoolean("disabled", false);

  std::unique_ptr<DocumentNode> child;

  child = node->GetChildNode("attribute_limits");
  if (child)
  {
    model->attribute_limits = ParseAttributeLimitsConfiguration(child);
  }

  child = node->GetChildNode("logger_provider");
  if (child)
  {
    model->logger_provider = ParseLoggerProviderConfiguration(child);
  }

  child = node->GetChildNode("meter_provider");
  if (child)
  {
    model->meter_provider = ParseMeterProviderConfiguration(child);
  }

  child = node->GetChildNode("propagator");
  if (child)
  {
    model->propagator = ParsePropagatorConfiguration(child);
  }

  child = node->GetChildNode("tracer_provider");
  if (child)
  {
    model->tracer_provider = ParseTracerProviderConfiguration(child);
  }

  child = node->GetChildNode("resource");
  if (child)
  {
    model->resource = ParseResourceConfiguration(child);
  }

  return model;
}

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
