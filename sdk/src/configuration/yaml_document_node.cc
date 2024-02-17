// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <yaml-cpp/yaml.h>

#include "opentelemetry/sdk/common/global_log_handler.h"

#include "opentelemetry/sdk/configuration/invalid_schema_exception.h"
#include "opentelemetry/sdk/configuration/yaml_document.h"
#include "opentelemetry/sdk/configuration/yaml_document_node.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

#if 0
static void DebugNode(std::string_view name, const YAML::Node &yaml)
{
  if (yaml)
  {
    OTEL_INTERNAL_LOG_DEBUG("Processing: " << name << ", IsDefined: " << yaml.IsDefined());
    OTEL_INTERNAL_LOG_DEBUG("Processing: " << name << ", Size: " << yaml.size());

    const char *msg;
    switch (yaml.Type())
    {
      case YAML::NodeType::Undefined:
        msg = "NodeType::Undefined";
        break;
      case YAML::NodeType::Null:
        msg = "NodeType::Null";
        break;
      case YAML::NodeType::Scalar:
        msg = "NodeType::Scalar";
        break;
      case YAML::NodeType::Sequence:
        msg = "NodeType::Sequence";
        break;
      case YAML::NodeType::Map:
        msg = "NodeType::Map";
        break;
      default:
        msg = "NodeType::???";
        break;
    }

    OTEL_INTERNAL_LOG_DEBUG("Processing: " << name << ", " << msg);
    OTEL_INTERNAL_LOG_DEBUG("Processing: " << name << ", Scalar: " << yaml.Scalar());
  }
  else
  {
    OTEL_INTERNAL_LOG_DEBUG("Processing: " << name << ", missing node.");
  }
}
#endif

bool YamlDocumentNode::AsBoolean()
{
  OTEL_INTERNAL_LOG_DEBUG("YamlDocumentNode::AsBoolean()");

  if (!m_yaml.IsScalar())
  {
    OTEL_INTERNAL_LOG_ERROR("Yaml: not scalar");
    throw InvalidSchemaException("");
  }
  bool value = m_yaml.as<bool>();
  return value;
}

size_t YamlDocumentNode::AsInteger()
{
  OTEL_INTERNAL_LOG_DEBUG("YamlDocumentNode::AsInteger()");

  if (!m_yaml.IsScalar())
  {
    OTEL_INTERNAL_LOG_ERROR("Yaml: not scalar");
    throw InvalidSchemaException("");
  }
  size_t value = m_yaml.as<size_t>();
  return value;
}

double YamlDocumentNode::AsDouble()
{
  OTEL_INTERNAL_LOG_DEBUG("YamlDocumentNode::AsDouble()");

  if (!m_yaml.IsScalar())
  {
    OTEL_INTERNAL_LOG_ERROR("Yaml: not scalar");
    throw InvalidSchemaException("");
  }
  double value = m_yaml.as<double>();
  return value;
}

std::string YamlDocumentNode::AsString()
{
  OTEL_INTERNAL_LOG_DEBUG("YamlDocumentNode::AsString()");

  if (!m_yaml.IsScalar())
  {
    OTEL_INTERNAL_LOG_ERROR("Yaml: not scalar");
    throw InvalidSchemaException("");
  }
  std::string value = m_yaml.as<std::string>();
  return value;
}

std::unique_ptr<DocumentNode> YamlDocumentNode::GetRequiredChildNode(const std::string &name)
{
  OTEL_INTERNAL_LOG_DEBUG("YamlDocumentNode::GetRequiredChildNode(" << name << ")");

  std::unique_ptr<DocumentNode> child;

  YAML::Node yaml_child = m_yaml[name];
  // DebugNode(name, yaml_child);
  if (!yaml_child)
  {
    OTEL_INTERNAL_LOG_ERROR("Yaml: required node: " << name);
    throw InvalidSchemaException(name);
  }
  child = std::unique_ptr<DocumentNode>(new YamlDocumentNode(yaml_child));
  return child;
}

std::unique_ptr<DocumentNode> YamlDocumentNode::GetChildNode(const std::string &name)
{
  OTEL_INTERNAL_LOG_DEBUG("YamlDocumentNode::GetChildNode(" << name << ")");

  std::unique_ptr<DocumentNode> child;

  YAML::Node yaml_child = m_yaml[name];
  // DebugNode(name, yaml_child);
  if (yaml_child)
  {
    child = std::unique_ptr<DocumentNode>(new YamlDocumentNode(yaml_child));
  }
  return child;
}

bool YamlDocumentNode::GetRequiredBoolean(const std::string &name)
{
  OTEL_INTERNAL_LOG_DEBUG("YamlDocumentNode::GetRequiredBoolean(" << name << ")");

  YAML::Node attr = m_yaml[name];
  if (!attr)
  {
    OTEL_INTERNAL_LOG_ERROR("Yaml: required node: " << name);
    throw InvalidSchemaException(name);
  }
  if (!attr.IsScalar())
  {
    OTEL_INTERNAL_LOG_ERROR("Yaml: bool node: " << name);
    throw InvalidSchemaException(name);
  }
  bool value = attr.as<bool>();
  return value;
}

bool YamlDocumentNode::GetBoolean(const std::string &name, bool default_value)
{
  OTEL_INTERNAL_LOG_DEBUG("YamlDocumentNode::GetBoolean(" << name << ", " << default_value << ")");

  bool value      = default_value;
  YAML::Node attr = m_yaml[name];
  if (attr)
  {
    if (!attr.IsScalar())
    {
      OTEL_INTERNAL_LOG_ERROR("Yaml: bool node: " << name);
      throw InvalidSchemaException(name);
    }
    value = attr.as<bool>();
  }
  return value;
}

size_t YamlDocumentNode::GetRequiredInteger(const std::string &name)
{
  OTEL_INTERNAL_LOG_DEBUG("YamlDocumentNode::GetRequiredInteger(" << name << ")");

  YAML::Node attr = m_yaml[name];
  if (!attr)
  {
    OTEL_INTERNAL_LOG_ERROR("Yaml: required node: " << name);
    throw InvalidSchemaException(name);
  }
  if (!attr.IsScalar())
  {
    OTEL_INTERNAL_LOG_ERROR("Yaml: integer node: " << name);
    throw InvalidSchemaException(name);
  }
  size_t value = attr.as<size_t>();
  return value;
}

size_t YamlDocumentNode::GetInteger(const std::string &name, size_t default_value)
{
  OTEL_INTERNAL_LOG_DEBUG("YamlDocumentNode::GetInteger(" << name << ", " << default_value << ")");

  size_t value    = default_value;
  YAML::Node attr = m_yaml[name];
  if (attr)
  {
    if (!attr.IsScalar())
    {
      OTEL_INTERNAL_LOG_ERROR("Yaml: integer node: " << name);
      throw InvalidSchemaException(name);
    }
    value = attr.as<size_t>();
  }
  return value;
}

double YamlDocumentNode::GetRequiredDouble(const std::string &name)
{
  OTEL_INTERNAL_LOG_DEBUG("YamlDocumentNode::GetRequiredDouble(" << name << ")");

  YAML::Node attr = m_yaml[name];
  if (!attr)
  {
    OTEL_INTERNAL_LOG_ERROR("Yaml: required node: " << name);
    throw InvalidSchemaException(name);
  }
  if (!attr.IsScalar())
  {
    OTEL_INTERNAL_LOG_ERROR("Yaml: double node: " << name);
    throw InvalidSchemaException(name);
  }
  double value = attr.as<double>();
  return value;
}

double YamlDocumentNode::GetDouble(const std::string &name, double default_value)
{
  OTEL_INTERNAL_LOG_DEBUG("YamlDocumentNode::GetDouble(" << name << ", " << default_value << ")");

  double value    = default_value;
  YAML::Node attr = m_yaml[name];
  if (attr)
  {
    if (!attr.IsScalar())
    {
      OTEL_INTERNAL_LOG_ERROR("Yaml: double node: " << name);
      throw InvalidSchemaException(name);
    }
    value = attr.as<double>();
  }
  return value;
}

std::string YamlDocumentNode::GetRequiredString(const std::string &name)
{
  OTEL_INTERNAL_LOG_DEBUG("YamlDocumentNode::GetRequiredString(" << name << ")");

  YAML::Node attr = m_yaml[name];
  if (!attr)
  {
    OTEL_INTERNAL_LOG_ERROR("Yaml: required node: " << name);
    throw InvalidSchemaException(name);
  }
  if (!attr.IsScalar())
  {
    OTEL_INTERNAL_LOG_ERROR("Yaml: string node: " << name);
    throw InvalidSchemaException(name);
  }
  std::string value = attr.as<std::string>();
  return value;
}

std::string YamlDocumentNode::GetString(const std::string &name, const std::string &default_value)
{
  OTEL_INTERNAL_LOG_DEBUG("YamlDocumentNode::GetString(" << name << ", " << default_value << ")");

  std::string value(default_value);
  YAML::Node attr = m_yaml[name];
  if (attr)
  {
    if (!attr.IsScalar())
    {
      OTEL_INTERNAL_LOG_ERROR("Yaml: string node: " << name);
      throw InvalidSchemaException(name);
    }
    value = attr.as<std::string>();
  }
  return value;
}

DocumentNodeConstIterator YamlDocumentNode::begin() const
{
  OTEL_INTERNAL_LOG_DEBUG("YamlDocumentNode::begin()");

  return DocumentNodeConstIterator(new YamlDocumentNodeConstIteratorImpl(m_yaml.begin()));
}

DocumentNodeConstIterator YamlDocumentNode::end() const
{
  OTEL_INTERNAL_LOG_DEBUG("YamlDocumentNode::end()");

  return DocumentNodeConstIterator(new YamlDocumentNodeConstIteratorImpl(m_yaml.end()));
}

size_t YamlDocumentNode::num_children() const
{
  OTEL_INTERNAL_LOG_ERROR("YamlDocumentNode::num_children(): FIXME");
  return 0;
}

std::unique_ptr<DocumentNode> YamlDocumentNode::GetChild(size_t index) const
{
  std::unique_ptr<DocumentNode> child;
  OTEL_INTERNAL_LOG_ERROR("YamlDocumentNode::GetChild(): FIXME");
  return child;
}

PropertiesNodeConstIterator YamlDocumentNode::begin_properties() const
{
  OTEL_INTERNAL_LOG_DEBUG("YamlDocumentNode::begin_properties()");

#if 0
  DebugNode("::begin_properties()", m_yaml);

  for (auto it = m_yaml.begin(); it != m_yaml.end(); ++it)
  {
    std::pair<YAML::Node, YAML::Node> pair = *it;
    DebugNode("<iter> first", pair.first);
    DebugNode("<iter> second", pair.second);
  }
#endif

  return PropertiesNodeConstIterator(new YamlPropertiesNodeConstIteratorImpl(m_yaml.begin()));
}

PropertiesNodeConstIterator YamlDocumentNode::end_properties() const
{
  OTEL_INTERNAL_LOG_DEBUG("YamlDocumentNode::end_properties()");

  return PropertiesNodeConstIterator(new YamlPropertiesNodeConstIteratorImpl(m_yaml.end()));
}

std::string YamlDocumentNode::Dump() const
{
  return "FIXME: Dump";
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
  return m_iter == other->m_iter;
}

YamlPropertiesNodeConstIteratorImpl::YamlPropertiesNodeConstIteratorImpl(
    const YAML::const_iterator &iter)
    : m_iter(iter)
{}

YamlPropertiesNodeConstIteratorImpl::~YamlPropertiesNodeConstIteratorImpl() {}

void YamlPropertiesNodeConstIteratorImpl::Next()
{
  m_iter++;
}

std::string YamlPropertiesNodeConstIteratorImpl::Name() const
{
  std::pair<std::string, std::unique_ptr<DocumentNode>> result;
  std::pair<YAML::Node, YAML::Node> kv = *m_iter;
  std::string name                     = kv.first.as<std::string>();

  OTEL_INTERNAL_LOG_DEBUG("YamlPropertiesNodeConstIteratorImpl::Name() = " << name);

  return name;
}

std::unique_ptr<DocumentNode> YamlPropertiesNodeConstIteratorImpl::Value() const
{
  std::unique_ptr<DocumentNode> item;
  std::pair<YAML::Node, YAML::Node> kv = *m_iter;
  item = std::unique_ptr<DocumentNode>(new YamlDocumentNode(kv.second));
  return item;
}

bool YamlPropertiesNodeConstIteratorImpl::Equal(const PropertiesNodeConstIteratorImpl *rhs) const
{
  const YamlPropertiesNodeConstIteratorImpl *other =
      static_cast<const YamlPropertiesNodeConstIteratorImpl *>(rhs);
  return m_iter == other->m_iter;
}

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
