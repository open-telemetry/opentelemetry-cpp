// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <stddef.h>
#include <stdio.h>
#include <cstddef>
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
#include "opentelemetry/sdk/configuration/attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/attributes_configuration.h"
#include "opentelemetry/sdk/configuration/base2_exponential_bucket_histogram_aggregation_configuration.h"
#include "opentelemetry/sdk/configuration/batch_log_record_processor_configuration.h"
#include "opentelemetry/sdk/configuration/batch_span_processor_configuration.h"
#include "opentelemetry/sdk/configuration/boolean_array_attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/boolean_attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/configuration.h"
#include "opentelemetry/sdk/configuration/configuration_parser.h"
#include "opentelemetry/sdk/configuration/console_log_record_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/console_push_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/console_span_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/default_aggregation_configuration.h"
#include "opentelemetry/sdk/configuration/default_histogram_aggregation.h"
#include "opentelemetry/sdk/configuration/document.h"
#include "opentelemetry/sdk/configuration/document_node.h"
#include "opentelemetry/sdk/configuration/double_array_attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/double_attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/drop_aggregation_configuration.h"
#include "opentelemetry/sdk/configuration/explicit_bucket_histogram_aggregation_configuration.h"
#include "opentelemetry/sdk/configuration/extension_log_record_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/extension_log_record_processor_configuration.h"
#include "opentelemetry/sdk/configuration/extension_metric_producer_configuration.h"
#include "opentelemetry/sdk/configuration/extension_pull_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/extension_push_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/extension_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/extension_span_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/extension_span_processor_configuration.h"
#include "opentelemetry/sdk/configuration/grpc_tls_configuration.h"
#include "opentelemetry/sdk/configuration/headers_configuration.h"
#include "opentelemetry/sdk/configuration/http_tls_configuration.h"
#include "opentelemetry/sdk/configuration/include_exclude_configuration.h"
#include "opentelemetry/sdk/configuration/instrument_type.h"
#include "opentelemetry/sdk/configuration/integer_array_attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/integer_attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/invalid_schema_exception.h"
#include "opentelemetry/sdk/configuration/jaeger_remote_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/last_value_aggregation_configuration.h"
#include "opentelemetry/sdk/configuration/log_record_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/log_record_limits_configuration.h"
#include "opentelemetry/sdk/configuration/log_record_processor_configuration.h"
#include "opentelemetry/sdk/configuration/logger_provider_configuration.h"
#include "opentelemetry/sdk/configuration/meter_provider_configuration.h"
#include "opentelemetry/sdk/configuration/metric_producer_configuration.h"
#include "opentelemetry/sdk/configuration/metric_reader_configuration.h"
#include "opentelemetry/sdk/configuration/open_census_metric_producer_configuration.h"
#include "opentelemetry/sdk/configuration/otlp_file_log_record_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/otlp_file_push_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/otlp_file_span_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/otlp_grpc_log_record_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/otlp_grpc_push_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/otlp_grpc_span_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/otlp_http_encoding.h"
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
#include "opentelemetry/sdk/configuration/string_array_attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/string_array_configuration.h"
#include "opentelemetry/sdk/configuration/string_attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/sum_aggregation_configuration.h"
#include "opentelemetry/sdk/configuration/temporality_preference.h"
#include "opentelemetry/sdk/configuration/trace_id_ratio_based_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/tracer_provider_configuration.h"
#include "opentelemetry/sdk/configuration/translation_strategy.h"
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

OtlpHttpEncoding ConfigurationParser::ParseOtlpHttpEncoding(
    const std::unique_ptr<DocumentNode> &node,
    const std::string &name) const
{
  if (name == "protobuf")
  {
    return OtlpHttpEncoding::protobuf;
  }

  if (name == "json")
  {
    return OtlpHttpEncoding::json;
  }

  std::string message("Illegal OtlpHttpEncoding: ");
  message.append(name);
  throw InvalidSchemaException(node->Location(), message);
}

std::unique_ptr<StringArrayConfiguration> ConfigurationParser::ParseStringArrayConfiguration(
    const std::unique_ptr<DocumentNode> &node) const
{
  auto model = std::make_unique<StringArrayConfiguration>();

  for (auto it = node->begin(); it != node->end(); ++it)
  {
    std::unique_ptr<DocumentNode> child(*it);

    std::string name = child->AsString();

    model->string_array.push_back(name);
  }

  return model;
}

std::unique_ptr<IncludeExcludeConfiguration> ConfigurationParser::ParseIncludeExcludeConfiguration(
    const std::unique_ptr<DocumentNode> &node) const
{
  auto model = std::make_unique<IncludeExcludeConfiguration>();
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

std::unique_ptr<HeadersConfiguration> ConfigurationParser::ParseHeadersConfiguration(
    const std::unique_ptr<DocumentNode> &node) const
{
  auto model = std::make_unique<HeadersConfiguration>();
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

std::unique_ptr<AttributeLimitsConfiguration>
ConfigurationParser::ParseAttributeLimitsConfiguration(
    const std::unique_ptr<DocumentNode> &node) const
{
  auto model = std::make_unique<AttributeLimitsConfiguration>();

  model->attribute_value_length_limit = node->GetInteger("attribute_value_length_limit", 4096);
  model->attribute_count_limit        = node->GetInteger("attribute_count_limit", 128);

  return model;
}

std::unique_ptr<HttpTlsConfiguration> ConfigurationParser::ParseHttpTlsConfiguration(
    const std::unique_ptr<DocumentNode> &node) const
{
  auto model = std::make_unique<HttpTlsConfiguration>();
  std::unique_ptr<DocumentNode> child;

  model->certificate_file        = node->GetString("certificate_file", "");
  model->client_key_file         = node->GetString("client_key_file", "");
  model->client_certificate_file = node->GetString("client_certificate_file", "");

  return model;
}

std::unique_ptr<GrpcTlsConfiguration> ConfigurationParser::ParseGrpcTlsConfiguration(
    const std::unique_ptr<DocumentNode> &node) const
{
  auto model = std::make_unique<GrpcTlsConfiguration>();
  std::unique_ptr<DocumentNode> child;

  model->certificate_file        = node->GetString("certificate_file", "");
  model->client_key_file         = node->GetString("client_key_file", "");
  model->client_certificate_file = node->GetString("client_certificate_file", "");
  model->insecure                = node->GetBoolean("insecure", false);

  return model;
}

std::unique_ptr<OtlpHttpLogRecordExporterConfiguration>
ConfigurationParser::ParseOtlpHttpLogRecordExporterConfiguration(
    const std::unique_ptr<DocumentNode> &node) const
{
  auto model = std::make_unique<OtlpHttpLogRecordExporterConfiguration>();
  std::unique_ptr<DocumentNode> child;

  model->endpoint = node->GetRequiredString("endpoint");

  child = node->GetChildNode("tls");
  if (child)
  {
    model->tls = ParseHttpTlsConfiguration(child);
  }

  child = node->GetChildNode("headers");
  if (child)
  {
    model->headers = ParseHeadersConfiguration(child);
  }

  model->headers_list = node->GetString("headers_list", "");
  model->compression  = node->GetString("compression", "");
  model->timeout      = node->GetInteger("timeout", 10000);

  std::string encoding = node->GetString("encoding", "protobuf");
  model->encoding      = ParseOtlpHttpEncoding(node, encoding);

  return model;
}

std::unique_ptr<OtlpGrpcLogRecordExporterConfiguration>
ConfigurationParser::ParseOtlpGrpcLogRecordExporterConfiguration(
    const std::unique_ptr<DocumentNode> &node) const
{
  auto model = std::make_unique<OtlpGrpcLogRecordExporterConfiguration>();
  std::unique_ptr<DocumentNode> child;

  model->endpoint = node->GetRequiredString("endpoint");

  child = node->GetChildNode("tls");
  if (child)
  {
    model->tls = ParseGrpcTlsConfiguration(child);
  }

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

std::unique_ptr<OtlpFileLogRecordExporterConfiguration>
ConfigurationParser::ParseOtlpFileLogRecordExporterConfiguration(
    const std::unique_ptr<DocumentNode> &node) const
{
  auto model = std::make_unique<OtlpFileLogRecordExporterConfiguration>();
  std::unique_ptr<DocumentNode> child;

  model->output_stream = node->GetString("output_stream", "");

  return model;
}

std::unique_ptr<ConsoleLogRecordExporterConfiguration>
ConfigurationParser::ParseConsoleLogRecordExporterConfiguration(
    const std::unique_ptr<DocumentNode> & /* node */) const
{
  auto model = std::make_unique<ConsoleLogRecordExporterConfiguration>();

  return model;
}

std::unique_ptr<ExtensionLogRecordExporterConfiguration>
ConfigurationParser::ParseExtensionLogRecordExporterConfiguration(
    const std::string &name,
    std::unique_ptr<DocumentNode> node) const
{
  auto model = std::make_unique<ExtensionLogRecordExporterConfiguration>();

  model->name = name;
  model->node = std::move(node);

  return model;
}

std::unique_ptr<LogRecordExporterConfiguration>
ConfigurationParser::ParseLogRecordExporterConfiguration(
    const std::unique_ptr<DocumentNode> &node) const
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
    std::string message("Illegal log record exporter, count: ");
    message.append(std::to_string(count));
    throw InvalidSchemaException(node->Location(), message);
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

std::unique_ptr<BatchLogRecordProcessorConfiguration>
ConfigurationParser::ParseBatchLogRecordProcessorConfiguration(
    const std::unique_ptr<DocumentNode> &node) const
{
  auto model = std::make_unique<BatchLogRecordProcessorConfiguration>();
  std::unique_ptr<DocumentNode> child;

  model->schedule_delay        = node->GetInteger("schedule_delay", 5000);
  model->export_timeout        = node->GetInteger("export_timeout", 30000);
  model->max_queue_size        = node->GetInteger("max_queue_size", 2048);
  model->max_export_batch_size = node->GetInteger("max_export_batch_size", 512);

  child           = node->GetRequiredChildNode("exporter");
  model->exporter = ParseLogRecordExporterConfiguration(child);

  return model;
}

std::unique_ptr<SimpleLogRecordProcessorConfiguration>
ConfigurationParser::ParseSimpleLogRecordProcessorConfiguration(
    const std::unique_ptr<DocumentNode> &node) const
{
  auto model = std::make_unique<SimpleLogRecordProcessorConfiguration>();
  std::unique_ptr<DocumentNode> child;

  child           = node->GetRequiredChildNode("exporter");
  model->exporter = ParseLogRecordExporterConfiguration(child);

  return model;
}

std::unique_ptr<ExtensionLogRecordProcessorConfiguration>
ConfigurationParser::ParseExtensionLogRecordProcessorConfiguration(
    const std::string &name,
    std::unique_ptr<DocumentNode> node) const
{
  auto model = std::make_unique<ExtensionLogRecordProcessorConfiguration>();

  model->name = name;
  model->node = std::move(node);

  return model;
}

std::unique_ptr<LogRecordProcessorConfiguration>
ConfigurationParser::ParseLogRecordProcessorConfiguration(
    const std::unique_ptr<DocumentNode> &node) const
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
    std::string message("Illegal log record processor, count: ");
    message.append(std::to_string(count));
    throw InvalidSchemaException(node->Location(), message);
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

std::unique_ptr<LogRecordLimitsConfiguration>
ConfigurationParser::ParseLogRecordLimitsConfiguration(
    const std::unique_ptr<DocumentNode> &node) const
{
  auto model = std::make_unique<LogRecordLimitsConfiguration>();

  model->attribute_value_length_limit = node->GetInteger("attribute_value_length_limit", 4096);
  model->attribute_count_limit        = node->GetInteger("attribute_count_limit", 128);

  return model;
}

std::unique_ptr<LoggerProviderConfiguration> ConfigurationParser::ParseLoggerProviderConfiguration(
    const std::unique_ptr<DocumentNode> &node) const
{
  auto model = std::make_unique<LoggerProviderConfiguration>();
  std::unique_ptr<DocumentNode> child;

  child = node->GetRequiredChildNode("processors");

  for (auto it = child->begin(); it != child->end(); ++it)
  {
    model->processors.push_back(ParseLogRecordProcessorConfiguration(*it));
  }

  size_t count = model->processors.size();
  if (count == 0)
  {
    std::string message("Illegal logger provider, 0 processors");
    throw InvalidSchemaException(child->Location(), message);
  }

  child = node->GetChildNode("limits");
  if (child)
  {
    model->limits = ParseLogRecordLimitsConfiguration(child);
  }

  return model;
}

DefaultHistogramAggregation ConfigurationParser::ParseDefaultHistogramAggregation(
    const std::unique_ptr<DocumentNode> &node,
    const std::string &name) const
{
  if (name == "explicit_bucket_histogram")
  {
    return DefaultHistogramAggregation::explicit_bucket_histogram;
  }

  if (name == "base2_exponential_bucket_histogram")
  {
    return DefaultHistogramAggregation::base2_exponential_bucket_histogram;
  }

  std::string message("Illegal default_histogram_aggregation: ");
  message.append(name);
  throw InvalidSchemaException(node->Location(), message);
}

TemporalityPreference ConfigurationParser::ParseTemporalityPreference(
    const std::unique_ptr<DocumentNode> &node,
    const std::string &name) const
{
  if (name == "cumulative")
  {
    return TemporalityPreference::cumulative;
  }

  if (name == "delta")
  {
    return TemporalityPreference::delta;
  }

  if (name == "low_memory")
  {
    return TemporalityPreference::low_memory;
  }

  std::string message("Illegal temporality preference: ");
  message.append(name);
  throw InvalidSchemaException(node->Location(), message);
}

std::unique_ptr<OtlpHttpPushMetricExporterConfiguration>
ConfigurationParser::ParseOtlpHttpPushMetricExporterConfiguration(
    const std::unique_ptr<DocumentNode> &node) const
{
  auto model = std::make_unique<OtlpHttpPushMetricExporterConfiguration>();
  std::unique_ptr<DocumentNode> child;

  model->endpoint = node->GetRequiredString("endpoint");

  child = node->GetChildNode("tls");
  if (child)
  {
    model->tls = ParseHttpTlsConfiguration(child);
  }

  child = node->GetChildNode("headers");
  if (child)
  {
    model->headers = ParseHeadersConfiguration(child);
  }

  model->headers_list = node->GetString("headers_list", "");
  model->compression  = node->GetString("compression", "");
  model->timeout      = node->GetInteger("timeout", 10000);

  std::string temporality_preference = node->GetString("temporality_preference", "cumulative");
  model->temporality_preference      = ParseTemporalityPreference(node, temporality_preference);

  std::string default_histogram_aggregation =
      node->GetString("default_histogram_aggregation", "explicit_bucket_histogram");
  model->default_histogram_aggregation =
      ParseDefaultHistogramAggregation(node, default_histogram_aggregation);

  std::string encoding = node->GetString("encoding", "protobuf");
  model->encoding      = ParseOtlpHttpEncoding(node, encoding);

  return model;
}

std::unique_ptr<OtlpGrpcPushMetricExporterConfiguration>
ConfigurationParser::ParseOtlpGrpcPushMetricExporterConfiguration(
    const std::unique_ptr<DocumentNode> &node) const
{
  auto model = std::make_unique<OtlpGrpcPushMetricExporterConfiguration>();
  std::unique_ptr<DocumentNode> child;

  model->endpoint = node->GetRequiredString("endpoint");

  child = node->GetChildNode("tls");
  if (child)
  {
    model->tls = ParseGrpcTlsConfiguration(child);
  }

  child = node->GetChildNode("headers");
  if (child)
  {
    model->headers = ParseHeadersConfiguration(child);
  }

  model->headers_list = node->GetString("headers_list", "");
  model->compression  = node->GetString("compression", "");
  model->timeout      = node->GetInteger("timeout", 10000);

  std::string temporality_preference = node->GetString("temporality_preference", "cumulative");
  model->temporality_preference      = ParseTemporalityPreference(node, temporality_preference);

  std::string default_histogram_aggregation =
      node->GetString("default_histogram_aggregation", "explicit_bucket_histogram");
  model->default_histogram_aggregation =
      ParseDefaultHistogramAggregation(node, default_histogram_aggregation);

  return model;
}

std::unique_ptr<OtlpFilePushMetricExporterConfiguration>
ConfigurationParser::ParseOtlpFilePushMetricExporterConfiguration(
    const std::unique_ptr<DocumentNode> &node) const
{
  auto model = std::make_unique<OtlpFilePushMetricExporterConfiguration>();
  std::unique_ptr<DocumentNode> child;

  model->output_stream = node->GetString("output_stream", "");

  std::string temporality_preference = node->GetString("temporality_preference", "cumulative");
  model->temporality_preference      = ParseTemporalityPreference(node, temporality_preference);

  std::string default_histogram_aggregation =
      node->GetString("default_histogram_aggregation", "explicit_bucket_histogram");
  model->default_histogram_aggregation =
      ParseDefaultHistogramAggregation(node, default_histogram_aggregation);

  return model;
}

std::unique_ptr<ConsolePushMetricExporterConfiguration>
ConfigurationParser::ParseConsolePushMetricExporterConfiguration(
    const std::unique_ptr<DocumentNode> & /* node */) const
{
  auto model = std::make_unique<ConsolePushMetricExporterConfiguration>();

  // FIXME-CONFIG: https://github.com/open-telemetry/opentelemetry-configuration/issues/242

  return model;
}

TranslationStrategy ConfigurationParser::ParseTranslationStrategy(
    const std::unique_ptr<DocumentNode> &node,
    const std::string &name) const
{
  if (name == "UnderscoreEscapingWithSuffixes")
  {
    return TranslationStrategy::UnderscoreEscapingWithSuffixes;
  }

  if (name == "UnderscoreEscapingWithoutSuffixes")
  {
    return TranslationStrategy::UnderscoreEscapingWithoutSuffixes;
  }

  if (name == "NoUTF8EscapingWithSuffixes")
  {
    return TranslationStrategy::NoUTF8EscapingWithSuffixes;
  }

  if (name == "NoTranslation")
  {
    return TranslationStrategy::NoTranslation;
  }

  std::string message("Illegal TranslationStrategy: ");
  message.append(name);
  throw InvalidSchemaException(node->Location(), message);
}

std::unique_ptr<PrometheusPullMetricExporterConfiguration>
ConfigurationParser::ParsePrometheusPullMetricExporterConfiguration(
    const std::unique_ptr<DocumentNode> &node) const
{
  auto model = std::make_unique<PrometheusPullMetricExporterConfiguration>();
  std::unique_ptr<DocumentNode> child;

  model->host               = node->GetString("host", "localhost");
  model->port               = node->GetInteger("port", 9464);
  model->without_scope_info = node->GetBoolean("without_scope_info", false);

  child = node->GetChildNode("with_resource_constant_labels");
  if (child)
  {
    model->with_resource_constant_labels = ParseIncludeExcludeConfiguration(child);
  }

  std::string translation_strategy =
      node->GetString("translation_strategy", "UnderscoreEscapingWithSuffixes");
  model->translation_strategy = ParseTranslationStrategy(node, translation_strategy);

  return model;
}

std::unique_ptr<ExtensionPushMetricExporterConfiguration>
ConfigurationParser::ParsePushMetricExporterExtensionConfiguration(
    const std::string &name,
    std::unique_ptr<DocumentNode> node) const
{
  auto model = std::make_unique<ExtensionPushMetricExporterConfiguration>();

  model->name = name;
  model->node = std::move(node);

  return model;
}

std::unique_ptr<ExtensionPullMetricExporterConfiguration>
ConfigurationParser::ParsePullMetricExporterExtensionConfiguration(
    const std::string &name,
    std::unique_ptr<DocumentNode> node) const
{
  auto model = std::make_unique<ExtensionPullMetricExporterConfiguration>();

  model->name = name;
  model->node = std::move(node);

  return model;
}

std::unique_ptr<PushMetricExporterConfiguration>
ConfigurationParser::ParsePushMetricExporterConfiguration(
    const std::unique_ptr<DocumentNode> &node) const
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
    std::string message("Illegal push metric exporter, count: ");
    message.append(std::to_string(count));
    throw InvalidSchemaException(node->Location(), message);
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

std::unique_ptr<PullMetricExporterConfiguration>
ConfigurationParser::ParsePullMetricExporterConfiguration(
    const std::unique_ptr<DocumentNode> &node) const
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
    std::string message("Illegal pull metric exporter, count: ");
    message.append(std::to_string(count));
    throw InvalidSchemaException(node->Location(), message);
  }

  if (name == "prometheus/development")
  {
    model = ParsePrometheusPullMetricExporterConfiguration(child);
  }
  else
  {
    model = ParsePullMetricExporterExtensionConfiguration(name, std::move(child));
  }

  return model;
}

std::unique_ptr<OpenCensusMetricProducerConfiguration>
ConfigurationParser::ParseOpenCensusMetricProducerConfiguration(
    const std::unique_ptr<DocumentNode> & /* node */) const
{
  auto model = std::make_unique<OpenCensusMetricProducerConfiguration>();

  return model;
}

std::unique_ptr<ExtensionMetricProducerConfiguration>
ConfigurationParser::ParseExtensionMetricProducerConfiguration(
    const std::string &name,
    std::unique_ptr<DocumentNode> node) const
{
  auto model = std::make_unique<ExtensionMetricProducerConfiguration>();

  model->name = name;
  model->node = std::move(node);

  return model;
}

std::unique_ptr<MetricProducerConfiguration> ConfigurationParser::ParseMetricProducerConfiguration(
    const std::unique_ptr<DocumentNode> &node) const
{
  std::unique_ptr<MetricProducerConfiguration> model;

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
    std::string message("Illegal metric producer, properties count: ");
    message.append(std::to_string(count));
    throw InvalidSchemaException(node->Location(), message);
  }

  if (name == "opencensus")
  {
    model = ParseOpenCensusMetricProducerConfiguration(child);
  }
  else
  {
    model = ParseExtensionMetricProducerConfiguration(name, std::move(child));
  }

  return model;
}

std::unique_ptr<PeriodicMetricReaderConfiguration>
ConfigurationParser::ParsePeriodicMetricReaderConfiguration(
    const std::unique_ptr<DocumentNode> &node) const
{
  auto model = std::make_unique<PeriodicMetricReaderConfiguration>();
  std::unique_ptr<DocumentNode> child;

  model->interval = node->GetInteger("interval", 5000);
  model->timeout  = node->GetInteger("timeout", 30000);

  child           = node->GetRequiredChildNode("exporter");
  model->exporter = ParsePushMetricExporterConfiguration(child);

  child = node->GetChildNode("producers");

  if (child)
  {
    for (auto it = child->begin(); it != child->end(); ++it)
    {
      model->producers.push_back(ParseMetricProducerConfiguration(*it));
    }
  }

  return model;
}

std::unique_ptr<PullMetricReaderConfiguration>
ConfigurationParser::ParsePullMetricReaderConfiguration(
    const std::unique_ptr<DocumentNode> &node) const
{
  auto model = std::make_unique<PullMetricReaderConfiguration>();
  std::unique_ptr<DocumentNode> child;

  child           = node->GetRequiredChildNode("exporter");
  model->exporter = ParsePullMetricExporterConfiguration(child);

  child = node->GetChildNode("producers");

  if (child)
  {
    for (auto it = child->begin(); it != child->end(); ++it)
    {
      model->producers.push_back(ParseMetricProducerConfiguration(*it));
    }
  }

  return model;
}

std::unique_ptr<MetricReaderConfiguration> ConfigurationParser::ParseMetricReaderConfiguration(
    const std::unique_ptr<DocumentNode> &node) const
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
    std::string message("Illegal metric reader, count: ");
    message.append(std::to_string(count));
    throw InvalidSchemaException(node->Location(), message);
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
    std::string message("Illegal metric reader: ");
    message.append(name);
    throw InvalidSchemaException(node->Location(), message);
  }

  return model;
}

InstrumentType ConfigurationParser::ParseInstrumentType(const std::unique_ptr<DocumentNode> &node,
                                                        const std::string &name) const
{
  if (name == "")
  {
    return InstrumentType::none;
  }

  if (name == "counter")
  {
    return InstrumentType::counter;
  }

  if (name == "histogram")
  {
    return InstrumentType::histogram;
  }

  if (name == "observable_counter")
  {
    return InstrumentType::observable_counter;
  }

  if (name == "observable_gauge")
  {
    return InstrumentType::observable_gauge;
  }

  if (name == "observable_up_down_counter")
  {
    return InstrumentType::observable_up_down_counter;
  }

  if (name == "up_down_counter")
  {
    return InstrumentType::up_down_counter;
  }

  std::string message("Illegal instrument type: ");
  message.append(name);
  throw InvalidSchemaException(node->Location(), message);
}

std::unique_ptr<ViewSelectorConfiguration> ConfigurationParser::ParseViewSelectorConfiguration(
    const std::unique_ptr<DocumentNode> &node) const
{
  auto model = std::make_unique<ViewSelectorConfiguration>();

  model->instrument_name = node->GetString("instrument_name", "");

  std::string instrument_type = node->GetString("instrument_type", "");
  model->instrument_type      = ParseInstrumentType(node, instrument_type);

  model->unit             = node->GetString("unit", "");
  model->meter_name       = node->GetString("meter_name", "");
  model->meter_version    = node->GetString("meter_version", "");
  model->meter_schema_url = node->GetString("meter_schema_url", "");

  return model;
}

std::unique_ptr<DefaultAggregationConfiguration>
ConfigurationParser::ParseDefaultAggregationConfiguration(
    const std::unique_ptr<DocumentNode> & /* node */) const
{
  auto model = std::make_unique<DefaultAggregationConfiguration>();

  return model;
}

std::unique_ptr<DropAggregationConfiguration>
ConfigurationParser::ParseDropAggregationConfiguration(
    const std::unique_ptr<DocumentNode> & /* node */) const
{
  auto model = std::make_unique<DropAggregationConfiguration>();

  return model;
}

std::unique_ptr<ExplicitBucketHistogramAggregationConfiguration>
ConfigurationParser::ParseExplicitBucketHistogramAggregationConfiguration(
    const std::unique_ptr<DocumentNode> &node) const
{
  auto model = std::make_unique<ExplicitBucketHistogramAggregationConfiguration>();
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

std::unique_ptr<Base2ExponentialBucketHistogramAggregationConfiguration>
ConfigurationParser::ParseBase2ExponentialBucketHistogramAggregationConfiguration(
    const std::unique_ptr<DocumentNode> &node) const
{
  auto model = std::make_unique<Base2ExponentialBucketHistogramAggregationConfiguration>();

  model->max_scale      = node->GetInteger("max_scale", 20);
  model->max_size       = node->GetInteger("max_size", 160);
  model->record_min_max = node->GetBoolean("record_min_max", true);

  return model;
}

std::unique_ptr<LastValueAggregationConfiguration>
ConfigurationParser::ParseLastValueAggregationConfiguration(
    const std::unique_ptr<DocumentNode> & /* node */) const
{
  auto model = std::make_unique<LastValueAggregationConfiguration>();

  return model;
}

std::unique_ptr<SumAggregationConfiguration> ConfigurationParser::ParseSumAggregationConfiguration(
    const std::unique_ptr<DocumentNode> & /* node */) const
{
  auto model = std::make_unique<SumAggregationConfiguration>();

  return model;
}

std::unique_ptr<AggregationConfiguration> ConfigurationParser::ParseAggregationConfiguration(
    const std::unique_ptr<DocumentNode> &node) const
{
  std::unique_ptr<AggregationConfiguration> model;
  std::unique_ptr<DocumentNode> child;

  size_t count = node->num_children();

  if (count != 1)
  {
    std::string message("Illegal aggregation, children: ");
    message.append(std::to_string(count));
    throw InvalidSchemaException(node->Location(), message);
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
    std::string message("Illegal aggregation: ");
    message.append(name);
    throw InvalidSchemaException(node->Location(), message);
  }

  return model;
}

std::unique_ptr<ViewStreamConfiguration> ConfigurationParser::ParseViewStreamConfiguration(
    const std::unique_ptr<DocumentNode> &node) const
{
  auto model = std::make_unique<ViewStreamConfiguration>();
  std::unique_ptr<DocumentNode> child;

  model->name                          = node->GetString("name", "");
  model->description                   = node->GetString("description", "");
  model->aggregation_cardinality_limit = node->GetInteger("aggregation_cardinality_limit", 0);

  child = node->GetChildNode("aggregation");
  if (child)
  {
    model->aggregation = ParseAggregationConfiguration(child);
  }

  child = node->GetChildNode("attribute_keys");
  if (child)
  {
    model->attribute_keys = ParseIncludeExcludeConfiguration(child);
  }

  return model;
}

std::unique_ptr<ViewConfiguration> ConfigurationParser::ParseViewConfiguration(
    const std::unique_ptr<DocumentNode> &node) const
{
  auto model = std::make_unique<ViewConfiguration>();
  std::unique_ptr<DocumentNode> child;

  child           = node->GetRequiredChildNode("selector");
  model->selector = ParseViewSelectorConfiguration(child);

  child         = node->GetRequiredChildNode("stream");
  model->stream = ParseViewStreamConfiguration(child);

  return model;
}

std::unique_ptr<MeterProviderConfiguration> ConfigurationParser::ParseMeterProviderConfiguration(
    const std::unique_ptr<DocumentNode> &node) const
{
  auto model = std::make_unique<MeterProviderConfiguration>();
  std::unique_ptr<DocumentNode> child;

  child = node->GetRequiredChildNode("readers");

  for (auto it = child->begin(); it != child->end(); ++it)
  {
    model->readers.push_back(ParseMetricReaderConfiguration(*it));
  }

  if (model->readers.size() == 0)
  {
    std::string message("Illegal meter provider, 0 readers");
    throw InvalidSchemaException(child->Location(), message);
  }

  child = node->GetChildNode("views");

  if (child != nullptr)
  {
    for (auto it = child->begin(); it != child->end(); ++it)
    {
      model->views.push_back(ParseViewConfiguration(*it));
    }
  }

  return model;
}

std::unique_ptr<PropagatorConfiguration> ConfigurationParser::ParsePropagatorConfiguration(
    const std::unique_ptr<DocumentNode> &node) const
{
  auto model = std::make_unique<PropagatorConfiguration>();

  std::unique_ptr<DocumentNode> child;
  child = node->GetChildNode("composite");
  std::string name;
  int num_child = 0;

  if (child)
  {
    for (auto it = child->begin(); it != child->end(); ++it)
    {
      // This is an entry in the composite array
      std::unique_ptr<DocumentNode> element(*it);
      num_child++;
      int count = 0;

      // Find out its name, we expect an object with a unique property.
      for (auto it2 = element->begin_properties(); it2 != element->end_properties(); ++it2)
      {
        name = it2.Name();
        count++;
      }

      if (count != 1)
      {
        std::string message("Illegal composite child ");
        message.append(std::to_string(num_child));
        message.append(", properties count: ");
        message.append(std::to_string(count));
        throw InvalidSchemaException(element->Location(), message);
      }

      model->composite.push_back(name);
    }
  }

  model->composite_list = node->GetString("composite_list", "");

  return model;
}

std::unique_ptr<SpanLimitsConfiguration> ConfigurationParser::ParseSpanLimitsConfiguration(
    const std::unique_ptr<DocumentNode> &node) const
{
  auto model = std::make_unique<SpanLimitsConfiguration>();

  model->attribute_value_length_limit = node->GetInteger("attribute_value_length_limit", 4096);
  model->attribute_count_limit        = node->GetInteger("attribute_count_limit", 128);
  model->event_count_limit            = node->GetInteger("event_count_limit", 128);
  model->link_count_limit             = node->GetInteger("link_count_limit", 128);
  model->event_attribute_count_limit  = node->GetInteger("event_attribute_count_limit", 128);
  model->link_attribute_count_limit   = node->GetInteger("link_attribute_count_limit", 128);

  return model;
}

std::unique_ptr<AlwaysOffSamplerConfiguration>
ConfigurationParser::ParseAlwaysOffSamplerConfiguration(
    const std::unique_ptr<DocumentNode> & /* node */,
    size_t /* depth */) const
{
  auto model = std::make_unique<AlwaysOffSamplerConfiguration>();

  return model;
}

std::unique_ptr<AlwaysOnSamplerConfiguration>
ConfigurationParser::ParseAlwaysOnSamplerConfiguration(
    const std::unique_ptr<DocumentNode> & /* node */,
    size_t /* depth */) const
{
  auto model = std::make_unique<AlwaysOnSamplerConfiguration>();

  return model;
}

// NOLINTBEGIN(misc-no-recursion)
std::unique_ptr<JaegerRemoteSamplerConfiguration>
ConfigurationParser::ParseJaegerRemoteSamplerConfiguration(
    const std::unique_ptr<DocumentNode> &node,
    size_t depth) const
{
  auto model = std::make_unique<JaegerRemoteSamplerConfiguration>();
  std::unique_ptr<DocumentNode> child;

  // Unclear if endpoint and interval are required/optional
  // FIXME-CONFIG: https://github.com/open-telemetry/opentelemetry-configuration/issues/238
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
std::unique_ptr<ParentBasedSamplerConfiguration>
ConfigurationParser::ParseParentBasedSamplerConfiguration(const std::unique_ptr<DocumentNode> &node,
                                                          size_t depth) const
{
  auto model = std::make_unique<ParentBasedSamplerConfiguration>();
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

std::unique_ptr<TraceIdRatioBasedSamplerConfiguration>
ConfigurationParser::ParseTraceIdRatioBasedSamplerConfiguration(
    const std::unique_ptr<DocumentNode> &node,
    size_t /* depth */) const
{
  auto model = std::make_unique<TraceIdRatioBasedSamplerConfiguration>();
  std::unique_ptr<DocumentNode> child;

  model->ratio = node->GetDouble("ratio", 0);

  return model;
}

std::unique_ptr<ExtensionSamplerConfiguration>
ConfigurationParser::ParseSamplerExtensionConfiguration(const std::string &name,
                                                        std::unique_ptr<DocumentNode> node,
                                                        size_t depth) const
{
  auto model = std::make_unique<ExtensionSamplerConfiguration>();

  model->name  = name;
  model->node  = std::move(node);
  model->depth = depth;

  return model;
}

// NOLINTBEGIN(misc-no-recursion)
std::unique_ptr<SamplerConfiguration> ConfigurationParser::ParseSamplerConfiguration(
    const std::unique_ptr<DocumentNode> &node,
    size_t depth) const
{
  /*
   * ParseSamplerConfiguration() is recursive,
   * enforce a limit to prevent attacks from yaml.
   */
  if (depth >= MAX_SAMPLER_DEPTH)
  {
    std::string message("Samplers nested too deeply: ");
    message.append(std::to_string(depth));
    throw InvalidSchemaException(node->Location(), message);
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
    std::string message("Illegal sampler, properties count: ");
    message.append(std::to_string(count));
    throw InvalidSchemaException(node->Location(), message);
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

std::unique_ptr<OtlpHttpSpanExporterConfiguration>
ConfigurationParser::ParseOtlpHttpSpanExporterConfiguration(
    const std::unique_ptr<DocumentNode> &node) const
{
  auto model = std::make_unique<OtlpHttpSpanExporterConfiguration>();
  std::unique_ptr<DocumentNode> child;

  model->endpoint = node->GetRequiredString("endpoint");

  child = node->GetChildNode("tls");
  if (child)
  {
    model->tls = ParseHttpTlsConfiguration(child);
  }

  child = node->GetChildNode("headers");
  if (child)
  {
    model->headers = ParseHeadersConfiguration(child);
  }

  model->headers_list = node->GetString("headers_list", "");
  model->compression  = node->GetString("compression", "");
  model->timeout      = node->GetInteger("timeout", 10000);

  std::string encoding = node->GetString("encoding", "protobuf");
  model->encoding      = ParseOtlpHttpEncoding(node, encoding);

  return model;
}

std::unique_ptr<OtlpGrpcSpanExporterConfiguration>
ConfigurationParser::ParseOtlpGrpcSpanExporterConfiguration(
    const std::unique_ptr<DocumentNode> &node) const
{
  auto model = std::make_unique<OtlpGrpcSpanExporterConfiguration>();
  std::unique_ptr<DocumentNode> child;

  model->endpoint = node->GetRequiredString("endpoint");

  child = node->GetChildNode("tls");
  if (child)
  {
    model->tls = ParseGrpcTlsConfiguration(child);
  }

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

std::unique_ptr<OtlpFileSpanExporterConfiguration>
ConfigurationParser::ParseOtlpFileSpanExporterConfiguration(
    const std::unique_ptr<DocumentNode> &node) const
{
  auto model = std::make_unique<OtlpFileSpanExporterConfiguration>();
  std::unique_ptr<DocumentNode> child;

  model->output_stream = node->GetString("output_stream", "");

  return model;
}

std::unique_ptr<ConsoleSpanExporterConfiguration>
ConfigurationParser::ParseConsoleSpanExporterConfiguration(
    const std::unique_ptr<DocumentNode> & /* node */) const
{
  auto model = std::make_unique<ConsoleSpanExporterConfiguration>();

  return model;
}

std::unique_ptr<ZipkinSpanExporterConfiguration>
ConfigurationParser::ParseZipkinSpanExporterConfiguration(
    const std::unique_ptr<DocumentNode> &node) const
{
  auto model = std::make_unique<ZipkinSpanExporterConfiguration>();

  model->endpoint = node->GetRequiredString("endpoint");
  model->timeout  = node->GetInteger("timeout", 10000);

  return model;
}

std::unique_ptr<ExtensionSpanExporterConfiguration>
ConfigurationParser::ParseExtensionSpanExporterConfiguration(
    const std::string &name,
    std::unique_ptr<DocumentNode> node) const
{
  auto model = std::make_unique<ExtensionSpanExporterConfiguration>();

  model->name = name;
  model->node = std::move(node);

  return model;
}

std::unique_ptr<SpanExporterConfiguration> ConfigurationParser::ParseSpanExporterConfiguration(
    const std::unique_ptr<DocumentNode> &node) const
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
    std::string message("Illegal span exporter, properties count: ");
    message.append(std::to_string(count));
    throw InvalidSchemaException(node->Location(), message);
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

std::unique_ptr<BatchSpanProcessorConfiguration>
ConfigurationParser::ParseBatchSpanProcessorConfiguration(
    const std::unique_ptr<DocumentNode> &node) const
{
  auto model = std::make_unique<BatchSpanProcessorConfiguration>();
  std::unique_ptr<DocumentNode> child;

  model->schedule_delay        = node->GetInteger("schedule_delay", 5000);
  model->export_timeout        = node->GetInteger("export_timeout", 30000);
  model->max_queue_size        = node->GetInteger("max_queue_size", 2048);
  model->max_export_batch_size = node->GetInteger("max_export_batch_size", 512);

  child           = node->GetRequiredChildNode("exporter");
  model->exporter = ParseSpanExporterConfiguration(child);

  return model;
}

std::unique_ptr<SimpleSpanProcessorConfiguration>
ConfigurationParser::ParseSimpleSpanProcessorConfiguration(
    const std::unique_ptr<DocumentNode> &node) const
{
  auto model = std::make_unique<SimpleSpanProcessorConfiguration>();
  std::unique_ptr<DocumentNode> child;

  child           = node->GetRequiredChildNode("exporter");
  model->exporter = ParseSpanExporterConfiguration(child);

  return model;
}

std::unique_ptr<ExtensionSpanProcessorConfiguration>
ConfigurationParser::ParseExtensionSpanProcessorConfiguration(
    const std::string &name,
    std::unique_ptr<DocumentNode> node) const
{
  auto model = std::make_unique<ExtensionSpanProcessorConfiguration>();

  model->name = name;
  model->node = std::move(node);

  return model;
}

std::unique_ptr<SpanProcessorConfiguration> ConfigurationParser::ParseSpanProcessorConfiguration(
    const std::unique_ptr<DocumentNode> &node) const
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
    std::string message("Illegal span processor, properties count: ");
    message.append(std::to_string(count));
    throw InvalidSchemaException(node->Location(), message);
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

std::unique_ptr<TracerProviderConfiguration> ConfigurationParser::ParseTracerProviderConfiguration(
    const std::unique_ptr<DocumentNode> &node) const
{
  auto model = std::make_unique<TracerProviderConfiguration>();
  std::unique_ptr<DocumentNode> child;

  child = node->GetRequiredChildNode("processors");

  for (auto it = child->begin(); it != child->end(); ++it)
  {
    model->processors.push_back(ParseSpanProcessorConfiguration(*it));
  }

  size_t count = model->processors.size();
  if (count == 0)
  {
    std::string message("Illegal tracer provider, 0 processors");
    throw InvalidSchemaException(node->Location(), message);
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

std::unique_ptr<StringAttributeValueConfiguration>
ConfigurationParser::ParseStringAttributeValueConfiguration(
    const std::unique_ptr<DocumentNode> &node) const
{
  auto model = std::make_unique<StringAttributeValueConfiguration>();

  model->value = node->AsString();

  return model;
}

std::unique_ptr<IntegerAttributeValueConfiguration>
ConfigurationParser::ParseIntegerAttributeValueConfiguration(
    const std::unique_ptr<DocumentNode> &node) const
{
  auto model = std::make_unique<IntegerAttributeValueConfiguration>();

  model->value = node->AsInteger();

  return model;
}

std::unique_ptr<DoubleAttributeValueConfiguration>
ConfigurationParser::ParseDoubleAttributeValueConfiguration(
    const std::unique_ptr<DocumentNode> &node) const
{
  auto model = std::make_unique<DoubleAttributeValueConfiguration>();

  model->value = node->AsDouble();

  return model;
}

std::unique_ptr<BooleanAttributeValueConfiguration>
ConfigurationParser::ParseBooleanAttributeValueConfiguration(
    const std::unique_ptr<DocumentNode> &node) const
{
  auto model = std::make_unique<BooleanAttributeValueConfiguration>();

  model->value = node->AsBoolean();

  return model;
}

std::unique_ptr<StringArrayAttributeValueConfiguration>
ConfigurationParser::ParseStringArrayAttributeValueConfiguration(
    const std::unique_ptr<DocumentNode> &node) const
{
  auto model = std::make_unique<StringArrayAttributeValueConfiguration>();

  for (auto it = node->begin(); it != node->end(); ++it)
  {
    std::unique_ptr<DocumentNode> child(*it);

    std::string value = child->AsString();

    model->value.push_back(value);
  }

  return model;
}

std::unique_ptr<IntegerArrayAttributeValueConfiguration>
ConfigurationParser::ParseIntegerArrayAttributeValueConfiguration(
    const std::unique_ptr<DocumentNode> &node) const
{
  auto model = std::make_unique<IntegerArrayAttributeValueConfiguration>();

  for (auto it = node->begin(); it != node->end(); ++it)
  {
    std::unique_ptr<DocumentNode> child(*it);

    std::size_t value = child->AsInteger();

    model->value.push_back(value);
  }

  return model;
}

std::unique_ptr<DoubleArrayAttributeValueConfiguration>
ConfigurationParser::ParseDoubleArrayAttributeValueConfiguration(
    const std::unique_ptr<DocumentNode> &node) const
{
  auto model = std::make_unique<DoubleArrayAttributeValueConfiguration>();

  for (auto it = node->begin(); it != node->end(); ++it)
  {
    std::unique_ptr<DocumentNode> child(*it);

    double value = child->AsDouble();

    model->value.push_back(value);
  }

  return model;
}

std::unique_ptr<BooleanArrayAttributeValueConfiguration>
ConfigurationParser::ParseBooleanArrayAttributeValueConfiguration(
    const std::unique_ptr<DocumentNode> &node) const
{
  auto model = std::make_unique<BooleanArrayAttributeValueConfiguration>();

  for (auto it = node->begin(); it != node->end(); ++it)
  {
    std::unique_ptr<DocumentNode> child(*it);

    bool value = child->AsBoolean();

    model->value.push_back(value);
  }

  return model;
}

std::unique_ptr<AttributesConfiguration> ConfigurationParser::ParseAttributesConfiguration(
    const std::unique_ptr<DocumentNode> &node) const
{
  auto model = std::make_unique<AttributesConfiguration>();
  std::unique_ptr<DocumentNode> child;

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

    std::unique_ptr<AttributeValueConfiguration> value_model;

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
      auto model_detail = ParseStringAttributeValueConfiguration(value_child);
      value_model       = std::move(model_detail);
    }
    else if (type == "bool")
    {
      auto model_detail = ParseBooleanAttributeValueConfiguration(value_child);
      value_model       = std::move(model_detail);
    }
    else if (type == "int")
    {
      auto model_detail = ParseIntegerAttributeValueConfiguration(value_child);
      value_model       = std::move(model_detail);
    }
    else if (type == "double")
    {
      auto model_detail = ParseDoubleAttributeValueConfiguration(value_child);
      value_model       = std::move(model_detail);
    }
    else if (type == "string_array")
    {
      auto model_detail = ParseStringArrayAttributeValueConfiguration(value_child);
      value_model       = std::move(model_detail);
    }
    else if (type == "bool_array")
    {
      auto model_detail = ParseBooleanArrayAttributeValueConfiguration(value_child);
      value_model       = std::move(model_detail);
    }
    else if (type == "int_array")
    {
      auto model_detail = ParseIntegerArrayAttributeValueConfiguration(value_child);
      value_model       = std::move(model_detail);
    }
    else if (type == "double_array")
    {
      auto model_detail = ParseDoubleArrayAttributeValueConfiguration(value_child);
      value_model       = std::move(model_detail);
    }
    else
    {
      std::string message("Illegal attribute type: ");
      message.append(type);
      throw InvalidSchemaException(node->Location(), message);
    }

    std::pair<std::string, std::unique_ptr<AttributeValueConfiguration>> entry(
        name, std::move(value_model));
    model->kv_map.insert(std::move(entry));
  }

  return model;
}

std::unique_ptr<ResourceConfiguration> ConfigurationParser::ParseResourceConfiguration(
    const std::unique_ptr<DocumentNode> &node) const
{
  auto model = std::make_unique<ResourceConfiguration>();
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
    model->detectors = ParseIncludeExcludeConfiguration(child);
  }

  return model;
}

std::unique_ptr<Configuration> ConfigurationParser::Parse(std::unique_ptr<Document> doc)
{
  std::unique_ptr<DocumentNode> node = doc->GetRootNode();

  auto model = std::make_unique<Configuration>(std::move(doc));

  model->file_format = node->GetRequiredString("file_format");

  {
    int count;
    int major;
    int minor;

    count = sscanf(model->file_format.c_str(), "%d.%d", &major, &minor);
    if (count != 2)
    {
      std::string message("Invalid file_format");
      throw InvalidSchemaException(node->Location(), message);
    }

    if (major != 1)
    {
      std::string message("Unsupported file_format, major = ");
      message.append(std::to_string(major));
      throw InvalidSchemaException(node->Location(), message);
    }

    if (minor != 0)
    {
      std::string message("Unsupported file_format, major = ");
      message.append(std::to_string(major));
      message.append(", minor = ");
      message.append(std::to_string(minor));
      throw InvalidSchemaException(node->Location(), message);
    }

    version_major_ = major;
    version_minor_ = minor;
  }

  model->disabled = node->GetBoolean("disabled", false);

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
