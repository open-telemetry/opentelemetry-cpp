// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <fstream>

// #define WITH_YAML_CPP

#include "opentelemetry/sdk/common/global_log_handler.h"

#include "opentelemetry/sdk/configuration/configuration.h"
#include "opentelemetry/sdk/configuration/configuration_factory.h"
#include "opentelemetry/sdk/configuration/ryml_document.h"
#include "opentelemetry/sdk/configuration/ryml_document_node.h"
#include "opentelemetry/sdk/configuration/yaml_configuration_factory.h"
#include "opentelemetry/version.h"

#ifdef WITH_YAML_CPP
#  include "opentelemetry/sdk/configuration/yaml_document.h"
#  include "opentelemetry/sdk/configuration/yaml_document_node.h"
#endif

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

std::unique_ptr<Configuration> YamlConfigurationFactory::ParseFile(std::string filename)
{
  std::unique_ptr<Configuration> conf;
  std::ifstream in(filename, std::ios::binary);
  if (!in.is_open())
  {
    OTEL_INTERNAL_LOG_ERROR("Failed to open yaml file <" << filename << ">.");
  }
  else
  {
    std::ostringstream content;
    content << in.rdbuf();
    conf = YamlConfigurationFactory::ParseString(content.str());
  }

  return conf;
}

#ifdef WITH_YAML_CPP
static std::unique_ptr<Document> YamlCppParse(const std::string &content)
{
  std::unique_ptr<Document> doc;

  try
  {
    doc = YamlDocument::Parse(content);
  }
  catch (const YAML::BadFile &e)
  {
    OTEL_INTERNAL_LOG_ERROR("Failed to parse yaml, " << e.what());
    return nullptr;
  }
  catch (...)
  {
    OTEL_INTERNAL_LOG_ERROR("Failed to parse yaml.");
    return nullptr;
  }

  return doc;
}
#endif

static std::unique_ptr<Document> RymlParse(const std::string &content)
{
  std::unique_ptr<Document> doc;

  try
  {
    doc = RymlDocument::Parse(content);
  }
  catch (...)
  {
    OTEL_INTERNAL_LOG_ERROR("Failed to parse yaml.");
    return nullptr;
  }

  return doc;
}

std::unique_ptr<Configuration> YamlConfigurationFactory::ParseString(std::string content)
{
  std::unique_ptr<Document> doc;
  std::unique_ptr<DocumentNode> root;
  std::unique_ptr<Configuration> config;

#ifdef WITH_YAML_CPP
  doc = YamlCppParse(content);
#else
  doc = RymlParse(content);
#endif

  try
  {
    if (doc)
    {
      config = ConfigurationFactory::ParseConfiguration(std::move(doc));
    }
  }
  catch (...)
  {
    OTEL_INTERNAL_LOG_ERROR("Failed to interpret yaml.");
  }

  return config;
}

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
