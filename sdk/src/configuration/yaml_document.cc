// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <yaml-cpp/yaml.h>

#include "opentelemetry/sdk/common/global_log_handler.h"

#include "opentelemetry/sdk/configuration/yaml_document.h"
#include "opentelemetry/sdk/configuration/yaml_document_node.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

std::unique_ptr<Document> YamlDocument::Parse(std::string content)
{
  YAML::Node yaml;
  std::unique_ptr<Document> doc;

  try
  {
    yaml = YAML::Load(content);
  }
  catch (const YAML::BadFile &e)
  {
    OTEL_INTERNAL_LOG_ERROR("Failed to load yaml, " << e.what());
    return doc;
  }
  catch (...)
  {
    OTEL_INTERNAL_LOG_ERROR("Failed to load yaml.");
    return doc;
  }

  if (yaml.Type() == YAML::NodeType::Undefined)
  {
    OTEL_INTERNAL_LOG_ERROR("Failed to load yaml.");
    return doc;
  }

  YamlDocument *yaml_doc = new YamlDocument(yaml);
  doc                    = std::unique_ptr<Document>(yaml_doc);
  return doc;
}

std::unique_ptr<DocumentNode> YamlDocument::GetRootNode()
{
  YamlDocumentNode *yaml_node = new YamlDocumentNode(m_root);
  std::unique_ptr<DocumentNode> node(yaml_node);
  return node;
}

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
