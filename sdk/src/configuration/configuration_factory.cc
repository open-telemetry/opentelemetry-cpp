// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <yaml-cpp/yaml.h>

#include "opentelemetry/sdk/common/global_log_handler.h"

#include "opentelemetry/sdk/configuration/batch_span_processor_configuration.h"
#include "opentelemetry/sdk/configuration/configuration.h"
#include "opentelemetry/sdk/configuration/configuration_factory.h"
#include "opentelemetry/sdk/configuration/otlp_span_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/simple_span_processor_configuration.h"
#include "opentelemetry/sdk/configuration/zipkin_span_exporter_configuration.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

static void DebugNode(std::string_view name, const YAML::Node &yaml)
{
  if (yaml)
  {
    OTEL_INTERNAL_LOG_ERROR("Processing: " << name << ", IsDefined: " << yaml.IsDefined());
    OTEL_INTERNAL_LOG_ERROR("Processing: " << name << ", Size: " << yaml.size());
    OTEL_INTERNAL_LOG_ERROR("Processing: " << name << ", Type: " << yaml.Type());
    OTEL_INTERNAL_LOG_ERROR("Processing: " << name << ", Scalar: " << yaml.Scalar());
  }
  else
  {
    OTEL_INTERNAL_LOG_ERROR("Processing: " << name << ", missing node.");
  }
}

static YAML::Node GetChild(std::string_view name, const YAML::Node &yaml)
{
  YAML::Node child = yaml[name];
  // DebugNode(name, child);
  return child;
}

static YAML::Node GetRequiredChild(std::string_view name, const YAML::Node &yaml)
{
  YAML::Node child = yaml[name];
  // DebugNode(name, child);
  if (!child)
  {
    OTEL_INTERNAL_LOG_ERROR("Yaml: required node: " << name);
    // FIXME: throw
  }
  return child;
}

static bool GetRequiredBoolean(std::string_view name, const YAML::Node &yaml)
{
  YAML::Node child = GetRequiredChild(name, yaml);
  if (!child.IsScalar())
  {
    OTEL_INTERNAL_LOG_ERROR("Yaml: boolean node: " << name);
    // FIXME: throw
  }
  bool value = child.as<bool>();
  return value;
}

static bool GetBoolean(std::string_view name, const YAML::Node &yaml, bool default_value)
{
  YAML::Node child = GetChild(name, yaml);
  bool value       = default_value;
  if (child)
  {
    if (!child.IsScalar())
    {
      OTEL_INTERNAL_LOG_ERROR("Yaml: boolean node: " << name);
      // FIXME: throw
    }
    bool value = child.as<bool>();
  }
  return value;
}

static size_t GetRequiredInteger(std::string_view name, const YAML::Node &yaml)
{
  YAML::Node child = GetRequiredChild(name, yaml);
  if (!child.IsScalar())
  {
    OTEL_INTERNAL_LOG_ERROR("Yaml: integer node: " << name);
    // FIXME: throw
  }
  size_t value = child.as<size_t>();
  return value;
}

static size_t GetInteger(std::string_view name, const YAML::Node &yaml, size_t default_value)
{
  YAML::Node child = GetChild(name, yaml);
  size_t value     = default_value;
  if (child)
  {
    if (!child.IsScalar())
    {
      OTEL_INTERNAL_LOG_ERROR("Yaml: integer node: " << name);
      // FIXME: throw
    }
    size_t value = child.as<size_t>();
  }
  return value;
}

static std::string GetRequiredString(std::string_view name, const YAML::Node &yaml)
{
  YAML::Node child = GetRequiredChild(name, yaml);
  if (!child.IsScalar())
  {
    OTEL_INTERNAL_LOG_ERROR("Yaml: string node: " << name);
    // FIXME: throw
  }
  std::string value = child.as<std::string>();
  return value;
}

static std::string GetString(std::string_view name,
                             const YAML::Node &yaml,
                             std::string default_value)
{
  YAML::Node child  = GetChild(name, yaml);
  std::string value = default_value;
  if (child)
  {
    if (!child.IsScalar())
    {
      OTEL_INTERNAL_LOG_ERROR("Yaml: string node: " << name);
      // FIXME: throw
    }
    std::string value = child.as<std::string>();
  }
  return value;
}

static std::unique_ptr<AttributeLimitConfiguration> ParseYamlAttributeLimitConfiguration(
    const YAML::Node &yaml)
{
  std::unique_ptr<AttributeLimitConfiguration> model(new AttributeLimitConfiguration);

  model->attribute_value_length_limit = GetInteger("attribute_value_length_limit", yaml, 4096);
  model->attribute_count_limit        = GetInteger("attribute_count_limit", yaml, 128);

  return model;
}

static std::unique_ptr<LoggerProviderConfiguration> ParseYamlLoggerProviderConfiguration(
    const YAML::Node &yaml)
{
  std::unique_ptr<LoggerProviderConfiguration> model(new LoggerProviderConfiguration);

  OTEL_INTERNAL_LOG_ERROR("ParseYamlLoggerProviderConfiguration: FIXME");

  return model;
}

static std::unique_ptr<MeterProviderConfiguration> ParseYamlMeterProviderConfiguration(
    const YAML::Node &yaml)
{
  std::unique_ptr<MeterProviderConfiguration> model(new MeterProviderConfiguration);

  OTEL_INTERNAL_LOG_ERROR("ParseYamlMeterProviderConfiguration: FIXME");

  return model;
}

static std::unique_ptr<PropagatorConfiguration> ParseYamlPropagatorConfiguration(
    const YAML::Node &yaml)
{
  std::unique_ptr<PropagatorConfiguration> model(new PropagatorConfiguration);

  OTEL_INTERNAL_LOG_ERROR("ParseYamlPropagatorConfiguration: FIXME");

  return model;
}

static std::unique_ptr<SpanLimitsConfiguration> ParseYamlSpanLimitsConfiguration(
    const YAML::Node &yaml)
{
  std::unique_ptr<SpanLimitsConfiguration> model(new SpanLimitsConfiguration);

  OTEL_INTERNAL_LOG_ERROR("ParseYamlSpanLimitsConfiguration: FIXME");

  return model;
}

static std::unique_ptr<SamplerConfiguration> ParseYamlSamplerConfiguration(const YAML::Node &yaml)
{
  std::unique_ptr<SamplerConfiguration> model(new SamplerConfiguration);

  OTEL_INTERNAL_LOG_ERROR("ParseYamlSamplerConfiguration: FIXME");

  return model;
}

static std::unique_ptr<HeadersConfiguration> ParseYamlHeadersConfiguration(const YAML::Node &yaml)
{
  std::unique_ptr<HeadersConfiguration> model(new HeadersConfiguration);

  OTEL_INTERNAL_LOG_ERROR("HeadersConfiguration: FIXME");

  return model;
}

static std::unique_ptr<OtlpSpanExporterConfiguration> ParseYamlOtlpSpanExporterConfiguration(
    const YAML::Node &yaml)
{
  std::unique_ptr<OtlpSpanExporterConfiguration> model(new OtlpSpanExporterConfiguration);

  model->protocol           = GetRequiredString("protocol", yaml);
  model->endpoint           = GetRequiredString("endpoint", yaml);
  model->certificate        = GetString("certificate", yaml, "");
  model->client_key         = GetString("client_key", yaml, "");
  model->client_certificate = GetString("client_certificate", yaml, "");

  YAML::Node n   = GetChild("headers", yaml);
  model->headers = ParseYamlHeadersConfiguration(n);

  model->compression = GetString("compression", yaml, "");
  model->timeout     = GetInteger("timeout", yaml, 10000);
  model->insecure    = GetBoolean("insecure", yaml, false);

  return model;
}

static std::unique_ptr<ZipkinSpanExporterConfiguration> ParseYamlZipkinSpanExporterConfiguration(
    const YAML::Node &yaml)
{
  std::unique_ptr<ZipkinSpanExporterConfiguration> model(new ZipkinSpanExporterConfiguration);

  OTEL_INTERNAL_LOG_ERROR("ZipkinSpanExporterConfiguration: FIXME");

  return model;
}

static std::unique_ptr<SpanExporterConfiguration> ParseYamlSpanExporterConfiguration(
    const YAML::Node &yaml)
{
  std::unique_ptr<SpanExporterConfiguration> model;

  for (const auto &kv : yaml)
  {
    std::string k = kv.first.as<std::string>();
    YAML::Node v  = kv.second;

    if (k == "otlp")
    {
      model = ParseYamlOtlpSpanExporterConfiguration(v);
    }
    else if (k == "zipkin")
    {
      model = ParseYamlZipkinSpanExporterConfiguration(v);
    }
    else
    {
#ifdef LATER
      model = ParseYamlSpanExporterExtensionConfiguration(k, v);
#endif
    }

    break;
  }

  return model;
}

static std::unique_ptr<BatchSpanProcessorConfiguration> ParseYamlBatchSpanProcessorConfiguration(
    const YAML::Node &yaml)
{
  std::unique_ptr<BatchSpanProcessorConfiguration> model(new BatchSpanProcessorConfiguration);

  model->schedule_delay        = GetInteger("schedule_delay", yaml, 5000);
  model->export_timeout        = GetInteger("export_timeout", yaml, 30000);
  model->max_queue_size        = GetInteger("max_queue_size", yaml, 2048);
  model->max_export_batch_size = GetInteger("max_export_batch_size", yaml, 512);

  YAML::Node n    = GetRequiredChild("exporter", yaml);
  model->exporter = ParseYamlSpanExporterConfiguration(n);

  return model;
}

static std::unique_ptr<SimpleSpanProcessorConfiguration> ParseYamlSimpleSpanProcessorConfiguration(
    const YAML::Node &yaml)
{
  std::unique_ptr<SimpleSpanProcessorConfiguration> model(new SimpleSpanProcessorConfiguration);

  OTEL_INTERNAL_LOG_ERROR("ParseYamlSimpleSpanProcessorConfiguration: FIXME");

  return model;
}

static std::unique_ptr<SpanProcessorConfiguration> ParseYamlSpanProcessorConfiguration(
    const YAML::Node &yaml)
{
  std::unique_ptr<SpanProcessorConfiguration> model;

  for (const auto &kv : yaml)
  {
    std::string k = kv.first.as<std::string>();
    YAML::Node v  = kv.second;

    if (k == "batch")
    {
      model = ParseYamlBatchSpanProcessorConfiguration(v);
    }
    else if (k == "simple")
    {
      model = ParseYamlSimpleSpanProcessorConfiguration(v);
    }
    else
    {
#ifdef LATER
      model = ParseYamlSpanProcessorExtensionConfiguration(k, v);
#endif
    }

    break;
  }

  return model;
}

static std::unique_ptr<TracerProviderConfiguration> ParseYamlTracerProviderConfiguration(
    const YAML::Node &yaml)
{
  std::unique_ptr<TracerProviderConfiguration> model(new TracerProviderConfiguration);

  {
    YAML::Node n = GetRequiredChild("processors", yaml);

    for (YAML::const_iterator it = n.begin(); it != n.end(); ++it)
    {
      OTEL_INTERNAL_LOG_ERROR("processor = " << *it);
      model->processors.push_back(ParseYamlSpanProcessorConfiguration(*it));
    }
  }

  {
    YAML::Node n = GetChild("limits", yaml);

    if (n)
    {
      model->limits = ParseYamlSpanLimitsConfiguration(n);
    }
    else
    {
      OTEL_INTERNAL_LOG_ERROR("Yaml: required node limits");
      return nullptr;
    }
  }

  {
    YAML::Node n = GetChild("sampler", yaml);

    if (n)
    {
      model->sampler = ParseYamlSamplerConfiguration(n);
    }
    else
    {
      OTEL_INTERNAL_LOG_ERROR("Yaml: required node sampler");
      return nullptr;
    }
  }

  return model;
}

static std::unique_ptr<ResourceConfiguration> ParseYamlResourceConfiguration(const YAML::Node &yaml)
{
  std::unique_ptr<ResourceConfiguration> model(new ResourceConfiguration);

  OTEL_INTERNAL_LOG_ERROR("ParseYamlResourceConfiguration: FIXME");

  return model;
}

static std::unique_ptr<Configuration> ParseYamlConfiguration(const YAML::Node &yaml)
{
  std::unique_ptr<Configuration> model(new Configuration);

  model->file_format = GetRequiredString("file_format", yaml);
  model->disabled    = GetBoolean("disabled", yaml, false);

  {
    YAML::Node n = GetChild("attribute_limits", yaml);

    if (n)
    {
      model->attribute_limits = ParseYamlAttributeLimitConfiguration(n);
    }
  }

  {
    YAML::Node n = GetChild("logger_provider", yaml);

    if (n)
    {
      model->logger_provider = ParseYamlLoggerProviderConfiguration(n);
    }
  }

  {
    YAML::Node n = GetChild("meter_provider", yaml);

    if (n)
    {
      model->meter_provider = ParseYamlMeterProviderConfiguration(n);
    }
  }

  {
    YAML::Node n = GetChild("propagator", yaml);

    if (n)
    {
      model->propagator = ParseYamlPropagatorConfiguration(n);
    }
  }

  {
    YAML::Node n = GetChild("tracer_provider", yaml);

    if (n)
    {
      model->tracer_provider = ParseYamlTracerProviderConfiguration(n);
    }
  }

  {
    YAML::Node n = GetChild("resource", yaml);

    if (n)
    {
      model->resource = ParseYamlResourceConfiguration(n);
    }
  }

  return model;
}

std::unique_ptr<Configuration> ConfigurationFactory::Parse(std::string file_path)
{
  YAML::Node yaml;
  std::unique_ptr<Configuration> config;

  try
  {
    yaml = YAML::LoadFile(file_path);
  }
  catch (YAML::BadFile e)
  {
    OTEL_INTERNAL_LOG_ERROR("Failed to parse yaml file <" << file_path << ">, " << e.what());
    return config;
  }
  catch (...)
  {
    OTEL_INTERNAL_LOG_ERROR("Failed to parse yaml file <" << file_path << ">");
    return config;
  }

  DebugNode("root", yaml);

  if (yaml.Type() == YAML::NodeType::Undefined)
  {
    OTEL_INTERNAL_LOG_ERROR("Failed to parse yaml file <" << file_path << ">");
    return config;
  }

  try
  {
    config = ParseYamlConfiguration(yaml);
  }
  catch (YAML::Exception e)
  {
    OTEL_INTERNAL_LOG_ERROR("Failed interpret yaml file <" << file_path << ">, " << e.what());
  }
  catch (...)
  {
    OTEL_INTERNAL_LOG_ERROR("Failed interpret yaml file <" << file_path << ">");
  }

  return config;
}

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
