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

std::pair<std::string, std::unique_ptr<DocumentNode>> YamlDocumentNode::GetNameAndContent()
{
  std::pair<std::string, std::unique_ptr<DocumentNode>> result;

  for (const auto &kv : m_yaml)
  {
    result.first  = kv.first.as<std::string>();
    result.second = std::unique_ptr<DocumentNode>(new YamlDocumentNode(kv.second));

    return result;
  }

  OTEL_INTERNAL_LOG_ERROR("FIXME");
  return result;
}

std::unique_ptr<DocumentNode> YamlDocumentNode::GetRequiredChildNode(std::string_view name)
{
  std::unique_ptr<DocumentNode> child;

  YAML::Node yaml_child = m_yaml[name];
  // DebugNode(name, yaml_child);
  if (!yaml_child)
  {
    OTEL_INTERNAL_LOG_ERROR("Yaml: required node: " << name);
    // FIXME: throw
  }
  child = std::unique_ptr<DocumentNode>(new YamlDocumentNode(yaml_child));
  return child;
}

std::unique_ptr<DocumentNode> YamlDocumentNode::GetChildNode(std::string_view name)
{
  std::unique_ptr<DocumentNode> child;

  YAML::Node yaml_child = m_yaml[name];
  // DebugNode(name, yaml_child);
  if (yaml_child)
  {
    child = std::unique_ptr<DocumentNode>(new YamlDocumentNode(yaml_child));
  }
  return child;
}

bool YamlDocumentNode::GetRequiredBoolean(std::string_view name)
{
  YAML::Node attr = m_yaml[name];
  if (!attr)
  {
    OTEL_INTERNAL_LOG_ERROR("Yaml: required node: " << name);
    // FIXME: throw
  }
  if (!attr.IsScalar())
  {
    OTEL_INTERNAL_LOG_ERROR("Yaml: bool node: " << name);
    // FIXME: throw
  }
  bool value = attr.as<bool>();
  return value;
}

bool YamlDocumentNode::GetBoolean(std::string_view name, bool default_value)
{
  bool value      = default_value;
  YAML::Node attr = m_yaml[name];
  if (attr)
  {
    if (!attr.IsScalar())
    {
      OTEL_INTERNAL_LOG_ERROR("Yaml: bool node: " << name);
      // FIXME: throw
    }
    value = attr.as<bool>();
  }
  return value;
}

size_t YamlDocumentNode::GetRequiredInteger(std::string_view name)
{
  YAML::Node attr = m_yaml[name];
  if (!attr)
  {
    OTEL_INTERNAL_LOG_ERROR("Yaml: required node: " << name);
    // FIXME: throw
  }
  if (!attr.IsScalar())
  {
    OTEL_INTERNAL_LOG_ERROR("Yaml: integer node: " << name);
    // FIXME: throw
  }
  size_t value = attr.as<size_t>();
  return value;
}

size_t YamlDocumentNode::GetInteger(std::string_view name, size_t default_value)
{
  size_t value    = default_value;
  YAML::Node attr = m_yaml[name];
  if (attr)
  {
    if (!attr.IsScalar())
    {
      OTEL_INTERNAL_LOG_ERROR("Yaml: integer node: " << name);
      // FIXME: throw
    }
    value = attr.as<size_t>();
  }
  return value;
}

std::string YamlDocumentNode::GetRequiredString(std::string_view name)
{
  YAML::Node attr = m_yaml[name];
  if (!attr)
  {
    OTEL_INTERNAL_LOG_ERROR("Yaml: required node: " << name);
    // FIXME: throw
  }
  if (!attr.IsScalar())
  {
    OTEL_INTERNAL_LOG_ERROR("Yaml: string node: " << name);
    // FIXME: throw
  }
  std::string value = attr.as<std::string>();
  return value;
}

std::string YamlDocumentNode::GetString(std::string_view name, std::string_view default_value)
{
  std::string value(default_value);
  YAML::Node attr = m_yaml[name];
  if (attr)
  {
    if (!attr.IsScalar())
    {
      OTEL_INTERNAL_LOG_ERROR("Yaml: string node: " << name);
      // FIXME: throw
    }
    value = attr.as<std::string>();
  }
  return value;
}

DocumentNodeConstIterator YamlDocumentNode::begin() const
{
#ifdef LATER
  DebugNode("::begin()", m_yaml);

  for (auto it = m_yaml.begin(); it != m_yaml.end(); ++it)
  {
    std::pair<YAML::Node, YAML::Node> pair = *it;
    DebugNode("<iter> first", pair.first);
    DebugNode("<iter> second", pair.second);
  }
#endif

  return DocumentNodeConstIterator(new YamlDocumentNodeConstIteratorImpl(m_yaml.begin()));
}

DocumentNodeConstIterator YamlDocumentNode::end() const
{
  return DocumentNodeConstIterator(new YamlDocumentNodeConstIteratorImpl(m_yaml.end()));
}

YamlDocumentNodeConstIteratorImpl::YamlDocumentNodeConstIteratorImpl(
    const YAML::const_iterator &iter)
    : m_iter(iter)
{}

YamlDocumentNodeConstIteratorImpl::~YamlDocumentNodeConstIteratorImpl() {}

void YamlDocumentNodeConstIteratorImpl::Next()
{
  m_iter++;
}

std::unique_ptr<DocumentNode> YamlDocumentNodeConstIteratorImpl::Item() const
{
  std::unique_ptr<DocumentNode> item;
  YAML::Node yaml_item = *m_iter;
  if (!yaml_item)
  {
    OTEL_INTERNAL_LOG_ERROR("iterator is lost ");
    // Throw
  }
  item = std::unique_ptr<DocumentNode>(new YamlDocumentNode(yaml_item));
  return item;
}

bool YamlDocumentNodeConstIteratorImpl::Equal(const DocumentNodeConstIteratorImpl *rhs) const
{
  const YamlDocumentNodeConstIteratorImpl *other =
      static_cast<const YamlDocumentNodeConstIteratorImpl *>(rhs);
  OTEL_INTERNAL_LOG_ERROR("Equal()");
  return m_iter == other->m_iter;
}

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
