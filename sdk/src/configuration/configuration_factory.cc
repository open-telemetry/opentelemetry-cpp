// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <fstream>

#include "opentelemetry/sdk/common/global_log_handler.h"

#include "opentelemetry/sdk/configuration/always_off_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/always_on_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/batch_span_processor_configuration.h"
#include "opentelemetry/sdk/configuration/composite_propagator_configuration.h"
#include "opentelemetry/sdk/configuration/configuration.h"
#include "opentelemetry/sdk/configuration/configuration_factory.h"
#include "opentelemetry/sdk/configuration/console_span_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/document.h"
#include "opentelemetry/sdk/configuration/document_node.h"
#include "opentelemetry/sdk/configuration/extension_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/extension_span_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/extension_span_processor_configuration.h"
#include "opentelemetry/sdk/configuration/invalid_schema_exception.h"
#include "opentelemetry/sdk/configuration/jaeger_remote_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/otlp_span_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/parent_based_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/propagator_configuration.h"
#include "opentelemetry/sdk/configuration/propagator_configuration_visitor.h"
#include "opentelemetry/sdk/configuration/simple_propagator_configuration.h"
#include "opentelemetry/sdk/configuration/simple_span_processor_configuration.h"
#include "opentelemetry/sdk/configuration/trace_id_ratio_based_sampler_configuration.h"
#include "opentelemetry/sdk/configuration/zipkin_span_exporter_configuration.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

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

static std::unique_ptr<MeterProviderConfiguration> ParseMeterProviderConfiguration(
    const std::unique_ptr<DocumentNode> & /* node */)
{
  std::unique_ptr<MeterProviderConfiguration> model(new MeterProviderConfiguration);

  OTEL_INTERNAL_LOG_ERROR("ParseMeterProviderConfiguration: FIXME");

  return model;
}

#ifdef NEVER
static std::unique_ptr<SimplePropagatorConfiguration> ParseSinglePropagatorConfiguration(
    const std::string &name)
{
  std::unique_ptr<SimplePropagatorConfiguration> model(new SimplePropagatorConfiguration);

  model->name = name;

  return model;
}
#endif

static std::unique_ptr<SimplePropagatorConfiguration> ParseSinglePropagatorConfiguration(
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
    return ParseSinglePropagatorConfiguration(child);
  }

  OTEL_INTERNAL_LOG_ERROR("ParsePropagatorConfiguration: illegal propagator");
  throw InvalidSchemaException("Illegal propagator");
  return nullptr;

#ifdef NEVER
  // This is the code that complies with the current model spec
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
    OTEL_INTERNAL_LOG_ERROR("ParsePropagatorConfiguration: count " << count);
    throw InvalidSchemaException("Illegal propagator");
  }

  if (name == "composite")
  {
    model = ParseCompositePropagatorConfiguration(child);
  }
  else
  {
    model = ParseSinglePropagatorConfiguration(name);
  }

  return model;
#endif
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
    std::string /* name */,
    const std::unique_ptr<DocumentNode> & /* node */)
{
  std::unique_ptr<SamplerConfiguration> model(new ExtensionSamplerConfiguration);

  OTEL_INTERNAL_LOG_ERROR("SamplerExtensionConfiguration: FIXME");

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
    model = ParseSamplerExtensionConfiguration(name, child);
  }

  return model;
}

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
    const std::unique_ptr<DocumentNode> & /* node */)
{
  std::unique_ptr<ExtensionSpanExporterConfiguration> model(new ExtensionSpanExporterConfiguration);

  OTEL_INTERNAL_LOG_ERROR("ParseExtensionSpanExporterConfiguration: FIXME");
  model->name = name;

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
    model = ParseExtensionSpanExporterConfiguration(name, child);
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
                                         const std::unique_ptr<DocumentNode> & /* node */)
{
  std::unique_ptr<ExtensionSpanProcessorConfiguration> model(
      new ExtensionSpanProcessorConfiguration);

  OTEL_INTERNAL_LOG_ERROR("ParseExtensionSpanProcessorConfiguration: FIXME");
  model->name = name;

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
    model = ParseExtensionSpanProcessorConfiguration(name, child);
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

  child         = node->GetRequiredChildNode("limits");
  model->limits = ParseSpanLimitsConfiguration(child);

  child          = node->GetRequiredChildNode("sampler");
  model->sampler = ParseSamplerConfiguration(child);

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
    if (name == "service.name")
    {
      model->service_name = name;
    }
    else
    {
      std::pair<std::string, std::string> entry(name, string_value);
      model->kv_map.insert(entry);
    }
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
    const std::unique_ptr<DocumentNode> &node)
{
  std::unique_ptr<Configuration> model(new Configuration);

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
