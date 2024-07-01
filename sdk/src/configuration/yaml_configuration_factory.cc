// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <fstream>

#include "opentelemetry/sdk/common/global_log_handler.h"

#include "opentelemetry/sdk/configuration/configuration.h"
#include "opentelemetry/sdk/configuration/configuration_factory.h"
#include "opentelemetry/sdk/configuration/ryml_document.h"
#include "opentelemetry/sdk/configuration/ryml_document_node.h"
#include "opentelemetry/sdk/configuration/yaml_configuration_factory.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

std::unique_ptr<Configuration> YamlConfigurationFactory::ParseFile(const std::string &filename)
{
  std::string input_file = filename;

  if (input_file.empty())
  {
    const char *env_var = std::getenv("OTEL_CONFIG_FILE");
    if (env_var != nullptr)
    {
      input_file = env_var;
    }
  }

  if (input_file.empty())
  {
    input_file = "config.yaml";
  }

  std::unique_ptr<Configuration> conf;
  std::ifstream in(input_file, std::ios::binary);
  if (!in.is_open())
  {
    OTEL_INTERNAL_LOG_ERROR("Failed to open yaml file <" << input_file << ">.");
  }
  else
  {
    std::ostringstream content;
    content << in.rdbuf();
    conf = YamlConfigurationFactory::ParseString(content.str());
  }

  return conf;
}

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

std::unique_ptr<Configuration> YamlConfigurationFactory::ParseString(const std::string &content)
{
  std::unique_ptr<Document> doc;
  std::unique_ptr<DocumentNode> root;
  std::unique_ptr<Configuration> config;

  doc = RymlParse(content);

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
