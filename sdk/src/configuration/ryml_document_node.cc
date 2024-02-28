// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/common/global_log_handler.h"

#include "opentelemetry/sdk/configuration/invalid_schema_exception.h"
#include "opentelemetry/sdk/configuration/ryml_document.h"
#include "opentelemetry/sdk/configuration/ryml_document_node.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

static void DebugNode(std::string_view name, ryml::ConstNodeRef node)
{
  OTEL_INTERNAL_LOG_DEBUG("Processing: " << name);
  OTEL_INTERNAL_LOG_DEBUG(" - valid() : " << node.valid());
  OTEL_INTERNAL_LOG_DEBUG(" - empty() : " << node.empty());
  OTEL_INTERNAL_LOG_DEBUG(" - is_container() : " << node.is_container());
  OTEL_INTERNAL_LOG_DEBUG(" - is_map() : " << node.is_map());
  OTEL_INTERNAL_LOG_DEBUG(" - is_seq() : " << node.is_seq());
  OTEL_INTERNAL_LOG_DEBUG(" - is_val() : " << node.is_val());
  OTEL_INTERNAL_LOG_DEBUG(" - is_keyval() : " << node.is_keyval());
  OTEL_INTERNAL_LOG_DEBUG(" - has_key() : " << node.has_key());
  OTEL_INTERNAL_LOG_DEBUG(" - has_val() : " << node.has_val());
  OTEL_INTERNAL_LOG_DEBUG(" - num_children() : " << node.num_children());
  if (node.has_key())
  {
    OTEL_INTERNAL_LOG_DEBUG(" - key() : " << node.key());
  }
  if (node.has_val())
  {
    OTEL_INTERNAL_LOG_DEBUG(" - val() : " << node.val());
  }
}

bool RymlDocumentNode::AsBoolean()
{
  OTEL_INTERNAL_LOG_DEBUG("RymlDocumentNode::AsBoolean()");

#ifdef LATER
  if (!m_yaml.IsScalar())
  {
    OTEL_INTERNAL_LOG_ERROR("Yaml: not scalar");
    throw InvalidSchemaException("");
  }
  bool value = m_yaml.as<bool>();
  return value;
#endif
  return false;
}

size_t RymlDocumentNode::AsInteger()
{
  OTEL_INTERNAL_LOG_DEBUG("RymlDocumentNode::AsInteger()");

#ifdef LATER
  if (!m_yaml.IsScalar())
  {
    OTEL_INTERNAL_LOG_ERROR("Yaml: not scalar");
    throw InvalidSchemaException("");
  }
  size_t value = m_yaml.as<size_t>();
  return value;
#endif
  return 0;
}

double RymlDocumentNode::AsDouble()
{
  OTEL_INTERNAL_LOG_DEBUG("RymlDocumentNode::AsDouble()");

#ifdef LATER
  if (!m_yaml.IsScalar())
  {
    OTEL_INTERNAL_LOG_ERROR("Yaml: not scalar");
    throw InvalidSchemaException("");
  }
  double value = m_yaml.as<double>();
  return value;
#endif
  return 0.0;
}

std::string RymlDocumentNode::AsString()
{
  OTEL_INTERNAL_LOG_DEBUG("RymlDocumentNode::AsString()");

  if (!m_node.is_val() && !m_node.is_keyval())
  {
    OTEL_INTERNAL_LOG_ERROR("Yaml: not scalar");
    throw InvalidSchemaException("");
  }
  ryml::csubstr view = m_node.val();
  std::string value(view.str, view.len);
  return value;
}

ryml::ConstNodeRef RymlDocumentNode::GetRequiredRymlChildNode(const std::string &name)
{
  if (!m_node.is_map())
  {
    OTEL_INTERNAL_LOG_ERROR("Yaml: not a map, looking for " << name);
    throw InvalidSchemaException(name);
  }

  const char *name_str = name.c_str();
  if (!m_node.has_child(name_str))
  {
    OTEL_INTERNAL_LOG_ERROR("Yaml: required node: " << name);
    throw InvalidSchemaException(name);
  }

  ryml::ConstNodeRef ryml_child = m_node[name_str];
  return ryml_child;
}

ryml::ConstNodeRef RymlDocumentNode::GetRymlChildNode(const std::string &name)
{
  if (!m_node.is_map())
  {
    return ryml::ConstNodeRef{};
  }

  const char *name_str = name.c_str();
  if (!m_node.has_child(name_str))
  {
    return ryml::ConstNodeRef{};
  }

  ryml::ConstNodeRef ryml_child = m_node[name_str];
  return ryml_child;
}

std::unique_ptr<DocumentNode> RymlDocumentNode::GetRequiredChildNode(const std::string &name)
{
  OTEL_INTERNAL_LOG_DEBUG("RymlDocumentNode::GetRequiredChildNode(" << name << ")");

  auto ryml_child = GetRequiredRymlChildNode(name);
  auto child      = std::unique_ptr<DocumentNode>(new RymlDocumentNode(ryml_child));
  return child;
}

std::unique_ptr<DocumentNode> RymlDocumentNode::GetChildNode(const std::string &name)
{
  OTEL_INTERNAL_LOG_DEBUG("RymlDocumentNode::GetChildNode(" << name << ")");

  std::unique_ptr<DocumentNode> child;

  if (!m_node.is_map())
  {
    OTEL_INTERNAL_LOG_ERROR("Yaml: not a map, looking for " << name);
    throw InvalidSchemaException(name);
  }

  const char *name_str = name.c_str();
  if (!m_node.has_child(name_str))
  {
    return child;
  }

  ryml::ConstNodeRef ryml_child = m_node[name_str];
  child                         = std::unique_ptr<DocumentNode>(new RymlDocumentNode(ryml_child));
  return child;
}

bool RymlDocumentNode::GetRequiredBoolean(const std::string &name)
{
  OTEL_INTERNAL_LOG_DEBUG("RymlDocumentNode::GetRequiredBoolean(" << name << ")");

  auto ryml_child = GetRequiredRymlChildNode(name);

  bool value;
  ryml_child >> value;
  return value;
}

bool RymlDocumentNode::GetBoolean(const std::string &name, bool default_value)
{
  OTEL_INTERNAL_LOG_DEBUG("RymlDocumentNode::GetBoolean(" << name << ", " << default_value << ")");

  auto ryml_child = GetRymlChildNode(name);

  if (!ryml_child.valid())
  {
    return default_value;
  }

  bool value;
  ryml_child >> value;
  return value;
}

size_t RymlDocumentNode::GetRequiredInteger(const std::string &name)
{
  OTEL_INTERNAL_LOG_DEBUG("RymlDocumentNode::GetRequiredInteger(" << name << ")");

  auto ryml_child = GetRequiredRymlChildNode(name);

  size_t value;
  ryml_child >> value;
  return value;
}

size_t RymlDocumentNode::GetInteger(const std::string &name, size_t default_value)
{
  OTEL_INTERNAL_LOG_DEBUG("RymlDocumentNode::GetInteger(" << name << ", " << default_value << ")");

  auto ryml_child = GetRymlChildNode(name);

  if (!ryml_child.valid())
  {
    return default_value;
  }

  size_t value;
  ryml_child >> value;
  return value;
}

double RymlDocumentNode::GetRequiredDouble(const std::string &name)
{
  OTEL_INTERNAL_LOG_DEBUG("RymlDocumentNode::GetRequiredDouble(" << name << ")");

  auto ryml_child = GetRequiredRymlChildNode(name);

  double value;
  ryml_child >> value;
  return value;
}

double RymlDocumentNode::GetDouble(const std::string &name, double default_value)
{
  OTEL_INTERNAL_LOG_DEBUG("RymlDocumentNode::GetDouble(" << name << ", " << default_value << ")");

  auto ryml_child = GetRymlChildNode(name);

  if (!ryml_child.valid())
  {
    return default_value;
  }

  double value;
  ryml_child >> value;
  return value;
}

std::string RymlDocumentNode::GetRequiredString(const std::string &name)
{
  OTEL_INTERNAL_LOG_DEBUG("RymlDocumentNode::GetRequiredString(" << name << ")");

  ryml::ConstNodeRef ryml_child = GetRequiredRymlChildNode(name);
  ryml::csubstr view            = ryml_child.val();
  std::string value(view.str, view.len);

  DoSubstitution(value);

  return value;
}

std::string RymlDocumentNode::GetString(const std::string &name, const std::string &default_value)
{
  OTEL_INTERNAL_LOG_DEBUG("RymlDocumentNode::GetString(" << name << ", " << default_value << ")");

  ryml::ConstNodeRef ryml_child = GetRymlChildNode(name);

  if (!ryml_child.valid())
  {
    return default_value;
  }

  ryml::csubstr view = ryml_child.val();
  std::string value(view.str, view.len);

  DoSubstitution(value);

  return value;
}

DocumentNodeConstIterator RymlDocumentNode::begin() const
{
  OTEL_INTERNAL_LOG_DEBUG("RymlDocumentNode::begin()");

#if 0
  DebugNode("::begin()", m_node);

  for (int index = 0; index < m_node.num_children(); index++)
  {
    DebugNode("(child)", m_node[index]);
  }
#endif

  return DocumentNodeConstIterator(new RymlDocumentNodeConstIteratorImpl(m_node, 0));
}

DocumentNodeConstIterator RymlDocumentNode::end() const
{
  OTEL_INTERNAL_LOG_DEBUG("RymlDocumentNode::end()");

  return DocumentNodeConstIterator(
      new RymlDocumentNodeConstIteratorImpl(m_node, m_node.num_children()));
}

size_t RymlDocumentNode::num_children() const
{
  return m_node.num_children();
}

std::unique_ptr<DocumentNode> RymlDocumentNode::GetChild(size_t index) const
{
  std::unique_ptr<DocumentNode> child;
  ryml::ConstNodeRef ryml_child = m_node[index];
  child                         = std::unique_ptr<DocumentNode>(new RymlDocumentNode(ryml_child));
  return child;
}

PropertiesNodeConstIterator RymlDocumentNode::begin_properties() const
{
  OTEL_INTERNAL_LOG_DEBUG("RymlDocumentNode::begin_properties()");

#if 0
  DebugNode("::begin_properties()", m_node);

  for (int index = 0; index < m_node.num_children(); index++)
  {
    DebugNode("(child)", m_node[index]);
  }
#endif

  return PropertiesNodeConstIterator(new RymlPropertiesNodeConstIteratorImpl(m_node, 0));
}

PropertiesNodeConstIterator RymlDocumentNode::end_properties() const
{
  OTEL_INTERNAL_LOG_DEBUG("RymlDocumentNode::end_properties()");

  return PropertiesNodeConstIterator(
      new RymlPropertiesNodeConstIteratorImpl(m_node, m_node.num_children()));
}

std::string RymlDocumentNode::Dump() const
{
  return "FIXME: Dump";
}

RymlDocumentNodeConstIteratorImpl::RymlDocumentNodeConstIteratorImpl(ryml::ConstNodeRef parent,
                                                                     size_t index)
    : m_parent(parent), m_index(index)
{}

RymlDocumentNodeConstIteratorImpl::~RymlDocumentNodeConstIteratorImpl() {}

void RymlDocumentNodeConstIteratorImpl::Next()
{
  ++m_index;
}

std::unique_ptr<DocumentNode> RymlDocumentNodeConstIteratorImpl::Item() const
{
  std::unique_ptr<DocumentNode> item;
  ryml::ConstNodeRef ryml_item = m_parent[m_index];
  if (!ryml_item.valid())
  {
    OTEL_INTERNAL_LOG_ERROR("iterator is lost ");
    // Throw
  }
  item = std::unique_ptr<DocumentNode>(new RymlDocumentNode(ryml_item));
  return item;
}

bool RymlDocumentNodeConstIteratorImpl::Equal(const DocumentNodeConstIteratorImpl *rhs) const
{
  const RymlDocumentNodeConstIteratorImpl *other =
      static_cast<const RymlDocumentNodeConstIteratorImpl *>(rhs);
  return m_index == other->m_index;
}

RymlPropertiesNodeConstIteratorImpl::RymlPropertiesNodeConstIteratorImpl(ryml::ConstNodeRef parent,
                                                                         size_t index)
    : m_parent(parent), m_index(index)
{}

RymlPropertiesNodeConstIteratorImpl::~RymlPropertiesNodeConstIteratorImpl() {}

void RymlPropertiesNodeConstIteratorImpl::Next()
{
  OTEL_INTERNAL_LOG_DEBUG("RymlPropertiesNodeConstIteratorImpl::Next()");
  ++m_index;
}

std::string RymlPropertiesNodeConstIteratorImpl::Name() const
{
  ryml::ConstNodeRef ryml_item = m_parent[m_index];
  ryml::csubstr k              = ryml_item.key();
  std::string name(k.str, k.len);

  OTEL_INTERNAL_LOG_DEBUG("RymlPropertiesNodeConstIteratorImpl::Name() = " << name);

  return name;
}

std::unique_ptr<DocumentNode> RymlPropertiesNodeConstIteratorImpl::Value() const
{
  std::unique_ptr<DocumentNode> item;

  ryml::ConstNodeRef ryml_item = m_parent[m_index];
  item                         = std::unique_ptr<DocumentNode>(new RymlDocumentNode(ryml_item));

  OTEL_INTERNAL_LOG_DEBUG("RymlPropertiesNodeConstIteratorImpl::Value()");

  return item;
}

bool RymlPropertiesNodeConstIteratorImpl::Equal(const PropertiesNodeConstIteratorImpl *rhs) const
{
  const RymlPropertiesNodeConstIteratorImpl *other =
      static_cast<const RymlPropertiesNodeConstIteratorImpl *>(rhs);
  return m_index == other->m_index;
}

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
