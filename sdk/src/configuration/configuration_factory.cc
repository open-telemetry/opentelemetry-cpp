// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <fstream>

#include "opentelemetry/sdk/common/global_log_handler.h"

#include "opentelemetry/sdk/configuration/aggregation_configuration.h"
#include "opentelemetry/sdk/configuration/always_off_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/always_on_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/base2_exponential_bucket_histogram_aggregation_configuration.h"
#include "opentelemetry/sdk/configuration/batch_span_processor_configuration.h"
#include "opentelemetry/sdk/configuration/composite_propagator_configuration.h"
#include "opentelemetry/sdk/configuration/configuration.h"
#include "opentelemetry/sdk/configuration/configuration_factory.h"
#include "opentelemetry/sdk/configuration/console_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/console_span_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/default_aggregation_configuration.h"
#include "opentelemetry/sdk/configuration/document.h"
#include "opentelemetry/sdk/configuration/document_node.h"
#include "opentelemetry/sdk/configuration/drop_aggregation_configuration.h"
#include "opentelemetry/sdk/configuration/explicit_bucket_histogram_aggregation_configuration.h"
#include "opentelemetry/sdk/configuration/extension_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/extension_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/extension_span_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/extension_span_processor_configuration.h"
#include "opentelemetry/sdk/configuration/invalid_schema_exception.h"
#include "opentelemetry/sdk/configuration/jaeger_remote_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/last_value_aggregation_configuration.h"
#include "opentelemetry/sdk/configuration/metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/metric_reader_configuration.h"
#include "opentelemetry/sdk/configuration/otlp_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/otlp_span_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/parent_based_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/periodic_metric_reader_configuration.h"
#include "opentelemetry/sdk/configuration/prometheus_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/propagator_configuration.h"
#include "opentelemetry/sdk/configuration/propagator_configuration_visitor.h"
#include "opentelemetry/sdk/configuration/pull_metric_reader_configuration.h"
#include "opentelemetry/sdk/configuration/simple_propagator_configuration.h"
#include "opentelemetry/sdk/configuration/simple_span_processor_configuration.h"
#include "opentelemetry/sdk/configuration/sum_aggregation_configuration.h"
#include "opentelemetry/sdk/configuration/trace_id_ratio_based_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/view_configuration.h"
#include "opentelemetry/sdk/configuration/zipkin_span_exporter_configuration.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

static std::unique_ptr<HeadersConfiguration> ParseHeadersConfiguration(
    const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<HeadersConfiguration> model(new HeadersConfiguration);

  for (auto it = node->begin_properties(); it != node->end_properties(); ++it)
  {
    std::string name                    = it.Name();
    std::unique_ptr<DocumentNode> child = it.Value();
    std::string string_value            = child->AsString();

    OTEL_INTERNAL_LOG_DEBUG("ParseHeadersConfiguration() name = " << name
                                                                  << ", value = " << string_value);
    std::pair<std::string, std::string> entry(name, string_value);
    model->kv_map.insert(entry);
  }

  return model;
}

static std::unique_ptr<AttributeLimitConfiguration> ParseAttributeLimitConfiguration(
    const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<AttributeLimitConfiguration> model(new AttributeLimitConfiguration);

  model->attribute_value_length_limit = node->GetInteger("attribute_value_length_limit", 4096);
  model->attribute_count_limit        = node->GetInteger("attribute_count_limit", 128);

  return model;
}

static std::unique_ptr<LoggerProviderConfiguration> ParseLoggerProviderConfiguration(
    const std::unique_ptr<DocumentNode> & /* node */)
{
  std::unique_ptr<LoggerProviderConfiguration> model(new LoggerProviderConfiguration);

  OTEL_INTERNAL_LOG_ERROR("ParseLoggerProviderConfiguration: FIXME");

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

static std::unique_ptr<OtlpMetricExporterConfiguration> ParseOtlpMetricExporterConfiguration(
    const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<OtlpMetricExporterConfiguration> model(new OtlpMetricExporterConfiguration);
  std::unique_ptr<DocumentNode> child;

  model->protocol           = node->GetRequiredString("protocol");
  model->endpoint           = node->GetRequiredString("endpoint");
  model->certificate        = node->GetString("certificate", "");
  model->client_key         = node->GetString("client_key", "");
  model->client_certificate = node->GetString("client_certificate", "");

  child = node->GetChildNode("headers");
  if (child)
  {
    model->headers = ParseHeadersConfiguration(child);
  }

  model->compression            = node->GetString("compression", "");
  model->timeout                = node->GetInteger("timeout", 10000);
  model->temporality_preference = node->GetString("temporality_preference", "");

  std::string default_histogram_aggregation = node->GetString("default_histogram_aggregation", "");
  model->default_histogram_aggregation =
      ParseDefaultHistogramAggregation(default_histogram_aggregation);

  model->insecure = node->GetBoolean("insecure", false);

  return model;
}

static std::unique_ptr<ConsoleMetricExporterConfiguration> ParseConsoleMetricExporterConfiguration(
    const std::unique_ptr<DocumentNode> & /* node */)
{
  std::unique_ptr<ConsoleMetricExporterConfiguration> model(new ConsoleMetricExporterConfiguration);

  return model;
}

static std::unique_ptr<PrometheusMetricExporterConfiguration>
ParsePrometheusMetricExporterConfiguration(const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<PrometheusMetricExporterConfiguration> model(
      new PrometheusMetricExporterConfiguration);

  model->host                = node->GetString("host", "localhost");
  model->port                = node->GetInteger("port", 9464);
  model->without_units       = node->GetBoolean("without_units", false);
  model->without_type_suffix = node->GetBoolean("without_type_suffix", false);
  model->without_scope_info  = node->GetBoolean("without_scope_info", false);

  return model;
}

static std::unique_ptr<ExtensionMetricExporterConfiguration>
ParseMetricExporterExtensionConfiguration(const std::string &name,
                                          std::unique_ptr<DocumentNode> node)
{
  auto extension  = new ExtensionMetricExporterConfiguration;
  extension->name = name;
  extension->node = std::move(node);
  std::unique_ptr<ExtensionMetricExporterConfiguration> model(extension);
  return model;
}

static std::unique_ptr<MetricExporterConfiguration> ParseMetricExporterConfiguration(
    const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<MetricExporterConfiguration> model;

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
    OTEL_INTERNAL_LOG_ERROR("ParseMetricExporterConfiguration: count " << count);
    throw InvalidSchemaException("Illegal metric exporter");
  }

  if (name == "otlp")
  {
    model = ParseOtlpMetricExporterConfiguration(child);
  }
  else if (name == "console")
  {
    model = ParseConsoleMetricExporterConfiguration(child);
  }
  else if (name == "prometheus")
  {
    model = ParsePrometheusMetricExporterConfiguration(child);
  }
  else
  {
    model = ParseMetricExporterExtensionConfiguration(name, std::move(child));
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
  model->exporter = ParseMetricExporterConfiguration(child);

  return model;
}

static std::unique_ptr<PullMetricReaderConfiguration> ParsePullMetricReaderConfiguration(
    const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<PullMetricReaderConfiguration> model(new PullMetricReaderConfiguration);
  std::unique_ptr<DocumentNode> child;

  child           = node->GetRequiredChildNode("exporter");
  model->exporter = ParseMetricExporterConfiguration(child);

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

static std::unique_ptr<SelectorConfiguration> ParseSelectorConfiguration(
    const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<SelectorConfiguration> model(new SelectorConfiguration);

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
    const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<DefaultAggregationConfiguration> model(new DefaultAggregationConfiguration);

  return model;
}

static std::unique_ptr<DropAggregationConfiguration> ParseDropAggregationConfiguration(
    const std::unique_ptr<DocumentNode> &node)
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
    const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<LastValueAggregationConfiguration> model(new LastValueAggregationConfiguration);

  return model;
}

static std::unique_ptr<SumAggregationConfiguration> ParseSumAggregationConfiguration(
    const std::unique_ptr<DocumentNode> &node)
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

static std::unique_ptr<StreamConfiguration> ParseStreamConfiguration(
    const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<StreamConfiguration> model(new StreamConfiguration);
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
    for (auto it = child->begin(); it != child->end(); ++it)
    {
      std::unique_ptr<DocumentNode> attribute_key(*it);

      std::string name = attribute_key->AsString();

      model->attribute_keys.push_back(name);
    }
  }

  return model;
}

static std::unique_ptr<ViewConfiguration> ParseViewConfiguration(
    const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<ViewConfiguration> model(new ViewConfiguration);
  std::unique_ptr<DocumentNode> child;

  child           = node->GetRequiredChildNode("selector");
  model->selector = ParseSelectorConfiguration(child);

  child         = node->GetRequiredChildNode("stream");
  model->stream = ParseStreamConfiguration(child);

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

static std::unique_ptr<SimplePropagatorConfiguration> ParseSimplePropagatorConfiguration(
    const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<SimplePropagatorConfiguration> model(new SimplePropagatorConfiguration);

  model->name = node->AsString();

  return model;
}

static std::unique_ptr<CompositePropagatorConfiguration> ParseCompositePropagatorConfiguration(
    const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<CompositePropagatorConfiguration> model(new CompositePropagatorConfiguration);

  for (auto it = node->begin(); it != node->end(); ++it)
  {
    std::unique_ptr<DocumentNode> child(*it);

    std::string name = child->AsString();

    model->names.push_back(name);
  }

  return model;
}

static std::unique_ptr<PropagatorConfiguration> ParsePropagatorConfiguration(
    const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<DocumentNode> child;

  child = node->GetChildNode("composite");
  if (child)
  {
    return ParseCompositePropagatorConfiguration(child);
  }

  // FIXME: File bug with opentelemetry-configuration
  child = node->GetChildNode("simple");
  if (child)
  {
    return ParseSimplePropagatorConfiguration(child);
  }

  OTEL_INTERNAL_LOG_ERROR("ParsePropagatorConfiguration: illegal propagator");
  throw InvalidSchemaException("Illegal propagator");
  return nullptr;
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
    const std::unique_ptr<DocumentNode> &node);

static std::unique_ptr<SamplerConfiguration> ParseAlwaysOffSamplerConfiguration(
    const std::unique_ptr<DocumentNode> & /* node */)
{
  std::unique_ptr<SamplerConfiguration> model(new AlwaysOffSamplerConfiguration);

  return model;
}

static std::unique_ptr<SamplerConfiguration> ParseAlwaysOnSamplerConfiguration(
    const std::unique_ptr<DocumentNode> & /* node */)
{
  std::unique_ptr<SamplerConfiguration> model(new AlwaysOnSamplerConfiguration);

  return model;
}

static std::unique_ptr<SamplerConfiguration> ParseJaegerRemoteSamplerConfiguration(
    const std::unique_ptr<DocumentNode> &node)
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
    model->initial_sampler = ParseSamplerConfiguration(child);
  }

  return model;
}

static std::unique_ptr<SamplerConfiguration> ParseParentBasedSamplerConfiguration(
    const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<ParentBasedSamplerConfiguration> model(new ParentBasedSamplerConfiguration);
  std::unique_ptr<DocumentNode> child;

  child = node->GetChildNode("root");
  if (child)
  {
    model->root = ParseSamplerConfiguration(child);
  }

  child = node->GetChildNode("remote_parent_sampled");
  if (child)
  {
    model->remote_parent_sampled = ParseSamplerConfiguration(child);
  }

  child = node->GetChildNode("remote_parent_not_sampled");
  if (child)
  {
    model->remote_parent_not_sampled = ParseSamplerConfiguration(child);
  }

  child = node->GetChildNode("local_parent_sampled");
  if (child)
  {
    model->local_parent_sampled = ParseSamplerConfiguration(child);
  }

  child = node->GetChildNode("local_parent_not_sampled");
  if (child)
  {
    model->local_parent_not_sampled = ParseSamplerConfiguration(child);
  }

  return model;
}

static std::unique_ptr<SamplerConfiguration> ParseTraceIdRatioBasedSamplerConfiguration(
    const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<TraceIdRatioBasedSamplerConfiguration> model(
      new TraceIdRatioBasedSamplerConfiguration);
  std::unique_ptr<DocumentNode> child;

  model->ratio = node->GetDouble("ratio", 0);

  return model;
}

static std::unique_ptr<SamplerConfiguration> ParseSamplerExtensionConfiguration(
    const std::string &name,
    std::unique_ptr<DocumentNode> node)
{
  auto extension  = new ExtensionSamplerConfiguration;
  extension->name = name;
  extension->node = std::move(node);
  std::unique_ptr<SamplerConfiguration> model(extension);
  return model;
}

static std::unique_ptr<SamplerConfiguration> ParseSamplerConfiguration(
    const std::unique_ptr<DocumentNode> &node)
{
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
    model = ParseAlwaysOffSamplerConfiguration(child);
  }
  else if (name == "always_on")
  {
    model = ParseAlwaysOnSamplerConfiguration(child);
  }
  else if (name == "jaeger_remote")
  {
    model = ParseJaegerRemoteSamplerConfiguration(child);
  }
  else if (name == "parent_based")
  {
    model = ParseParentBasedSamplerConfiguration(child);
  }
  else if (name == "trace_id_ratio_based")
  {
    model = ParseTraceIdRatioBasedSamplerConfiguration(child);
  }
  else
  {
    model = ParseSamplerExtensionConfiguration(name, std::move(child));
  }

  return model;
}

static std::unique_ptr<OtlpSpanExporterConfiguration> ParseOtlpSpanExporterConfiguration(
    const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<OtlpSpanExporterConfiguration> model(new OtlpSpanExporterConfiguration);
  std::unique_ptr<DocumentNode> child;

  model->protocol           = node->GetRequiredString("protocol");
  model->endpoint           = node->GetRequiredString("endpoint");
  model->certificate        = node->GetString("certificate", "");
  model->client_key         = node->GetString("client_key", "");
  model->client_certificate = node->GetString("client_certificate", "");

  child = node->GetChildNode("headers");
  if (child)
  {
    model->headers = ParseHeadersConfiguration(child);
  }

  model->compression = node->GetString("compression", "");
  model->timeout     = node->GetInteger("timeout", 10000);
  model->insecure    = node->GetBoolean("insecure", false);

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
  auto extension  = new ExtensionSpanExporterConfiguration;
  extension->name = name;
  extension->node = std::move(node);
  std::unique_ptr<ExtensionSpanExporterConfiguration> model(extension);
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

  if (name == "otlp")
  {
    model = ParseOtlpSpanExporterConfiguration(child);
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
  auto extension  = new ExtensionSpanProcessorConfiguration;
  extension->name = name;
  extension->node = std::move(node);
  std::unique_ptr<ExtensionSpanProcessorConfiguration> model(extension);
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
    model->sampler = ParseSamplerConfiguration(child);
  }

  return model;
}

static std::unique_ptr<AttributesConfiguration> ParseAttributesConfiguration(
    const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<AttributesConfiguration> model(new AttributesConfiguration);
  std::unique_ptr<DocumentNode> child;

  for (auto it = node->begin_properties(); it != node->end_properties(); ++it)
  {
    std::string name                    = it.Name();
    std::unique_ptr<DocumentNode> child = it.Value();
    std::string string_value            = child->AsString();

    OTEL_INTERNAL_LOG_DEBUG(
        "ParseAttributesConfiguration() name = " << name << ", value = " << string_value);

    std::pair<std::string, std::string> entry(name, string_value);
    model->kv_map.insert(entry);
  }

  return model;
}

static std::unique_ptr<ResourceConfiguration> ParseResourceConfiguration(
    const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<ResourceConfiguration> model(new ResourceConfiguration);
  std::unique_ptr<DocumentNode> child;

  model->schema_url = node->GetString("schema_url", "");

  child = node->GetRequiredChildNode("attributes");
  if (child)
  {
    model->attributes = ParseAttributesConfiguration(child);
  }

  return model;
}

std::unique_ptr<Configuration> ConfigurationFactory::ParseConfiguration(
    std::unique_ptr<Document> doc)
{
  std::unique_ptr<DocumentNode> node = doc->GetRootNode();
  std::unique_ptr<Configuration> model(new Configuration(std::move(doc)));

  model->file_format = node->GetRequiredString("file_format");
  model->disabled    = node->GetBoolean("disabled", false);

  std::unique_ptr<DocumentNode> child;

  child = node->GetChildNode("attribute_limits");
  if (child)
  {
    model->attribute_limits = ParseAttributeLimitConfiguration(child);
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
