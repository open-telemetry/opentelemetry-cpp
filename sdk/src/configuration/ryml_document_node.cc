// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <stddef.h>
#include <memory>
#include <ostream>
#include <ryml.hpp>
#include <string>
#include <utility>

#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/configuration/document_node.h"
#include "opentelemetry/sdk/configuration/invalid_schema_exception.h"
#include "opentelemetry/sdk/configuration/ryml_document_node.h"
#include "opentelemetry/version.h"

// Local debug, do not use in production
// #define WITH_DEBUG_NODE

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

#ifdef WITH_DEBUG_NODE
static void DebugNode(opentelemetry::nostd::string_view name, ryml::ConstNodeRef node)
{
  OTEL_INTERNAL_LOG_DEBUG("Processing: " << name);
  OTEL_INTERNAL_LOG_DEBUG(" - readable() : " << node.readable());
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
#endif  // WITH_DEBUG_NODE

std::string RymlDocumentNode::Key() const
{
  OTEL_INTERNAL_LOG_DEBUG("RymlDocumentNode::Key()");

  if (!node_.has_key())
  {
    OTEL_INTERNAL_LOG_ERROR("Yaml: no key");
    throw InvalidSchemaException("");
  }

  ryml::csubstr k = node_.key();
  std::string name(k.str, k.len);
  return name;
}

bool RymlDocumentNode::AsBoolean()
{
  OTEL_INTERNAL_LOG_DEBUG("RymlDocumentNode::AsBoolean()");

  if (!node_.is_val() && !node_.is_keyval())
  {
    OTEL_INTERNAL_LOG_ERROR("Yaml: not scalar");
    throw InvalidSchemaException("");
  }
  ryml::csubstr view = node_.val();
  std::string value(view.str, view.len);
  return BooleanFromString(value);
}

size_t RymlDocumentNode::AsInteger()
{
  OTEL_INTERNAL_LOG_DEBUG("RymlDocumentNode::AsInteger()");

  if (!node_.is_val() && !node_.is_keyval())
  {
    OTEL_INTERNAL_LOG_ERROR("Yaml: not scalar");
    throw InvalidSchemaException("");
  }
  ryml::csubstr view = node_.val();
  std::string value(view.str, view.len);
  return IntegerFromString(value);
}

double RymlDocumentNode::AsDouble()
{
  OTEL_INTERNAL_LOG_DEBUG("RymlDocumentNode::AsDouble()");

  if (!node_.is_val() && !node_.is_keyval())
  {
    OTEL_INTERNAL_LOG_ERROR("Yaml: not scalar");
    throw InvalidSchemaException("");
  }
  ryml::csubstr view = node_.val();
  std::string value(view.str, view.len);
  return DoubleFromString(value);
}

std::string RymlDocumentNode::AsString()
{
  OTEL_INTERNAL_LOG_DEBUG("RymlDocumentNode::AsString()");

  if (!node_.is_val() && !node_.is_keyval())
  {
    OTEL_INTERNAL_LOG_ERROR("Yaml: not scalar");
    throw InvalidSchemaException("");
  }
  ryml::csubstr view = node_.val();
  std::string value(view.str, view.len);
  return value;
}

ryml::ConstNodeRef RymlDocumentNode::GetRequiredRymlChildNode(const std::string &name)
{
  if (!node_.is_map())
  {
    OTEL_INTERNAL_LOG_ERROR("Yaml: not a map, looking for " << name);
    throw InvalidSchemaException(name);
  }

  const char *name_str = name.c_str();
  if (!node_.has_child(name_str))
  {
    OTEL_INTERNAL_LOG_ERROR("Yaml: required node: " << name);
    throw InvalidSchemaException(name);
  }

  ryml::ConstNodeRef ryml_child = node_[name_str];
  return ryml_child;
}

ryml::ConstNodeRef RymlDocumentNode::GetRymlChildNode(const std::string &name)
{
  if (!node_.is_map())
  {
    return ryml::ConstNodeRef{};
  }

  const char *name_str = name.c_str();
  if (!node_.has_child(name_str))
  {
    return ryml::ConstNodeRef{};
  }

  ryml::ConstNodeRef ryml_child = node_[name_str];
  return ryml_child;
}

std::unique_ptr<DocumentNode> RymlDocumentNode::GetRequiredChildNode(const std::string &name)
{
  OTEL_INTERNAL_LOG_DEBUG("RymlDocumentNode::GetRequiredChildNode(" << depth_ << ", " << name
                                                                    << ")");

  if (depth_ >= MAX_NODE_DEPTH)
  {
    OTEL_INTERNAL_LOG_ERROR("Yaml nested too deeply " << name);
    throw InvalidSchemaException(name);
  }

  auto ryml_child = GetRequiredRymlChildNode(name);
  auto child      = std::unique_ptr<DocumentNode>(new RymlDocumentNode(ryml_child, depth_ + 1));
  return child;
}

std::unique_ptr<DocumentNode> RymlDocumentNode::GetChildNode(const std::string &name)
{
  OTEL_INTERNAL_LOG_DEBUG("RymlDocumentNode::GetChildNode(" << depth_ << ", " << name << ")");

  if (depth_ >= MAX_NODE_DEPTH)
  {
    OTEL_INTERNAL_LOG_ERROR("Yaml nested too deeply " << name);
    throw InvalidSchemaException(name);
  }

  std::unique_ptr<DocumentNode> child;

  if (!node_.is_map())
  {
    return child;
  }

  const char *name_str = name.c_str();
  if (!node_.has_child(name_str))
  {
    return child;
  }

  ryml::ConstNodeRef ryml_child = node_[name_str];
  child = std::unique_ptr<DocumentNode>(new RymlDocumentNode(ryml_child, depth_ + 1));
  return child;
}

bool RymlDocumentNode::GetRequiredBoolean(const std::string &name)
{
  OTEL_INTERNAL_LOG_DEBUG("RymlDocumentNode::GetRequiredBoolean(" << name << ")");

  auto ryml_child = GetRequiredRymlChildNode(name);

  ryml::csubstr view = ryml_child.val();
  std::string value(view.str, view.len);

  value = DoSubstitution(value);

  return BooleanFromString(value);
}

bool RymlDocumentNode::GetBoolean(const std::string &name, bool default_value)
{
  OTEL_INTERNAL_LOG_DEBUG("RymlDocumentNode::GetBoolean(" << name << ", " << default_value << ")");

  auto ryml_child = GetRymlChildNode(name);

  if (ryml_child.invalid())
  {
    return default_value;
  }

  ryml::csubstr view = ryml_child.val();
  std::string value(view.str, view.len);

  value = DoSubstitution(value);

  if (value.empty())
  {
    return default_value;
  }

  return BooleanFromString(value);
}

size_t RymlDocumentNode::GetRequiredInteger(const std::string &name)
{
  OTEL_INTERNAL_LOG_DEBUG("RymlDocumentNode::GetRequiredInteger(" << name << ")");

  auto ryml_child = GetRequiredRymlChildNode(name);

  ryml::csubstr view = ryml_child.val();
  std::string value(view.str, view.len);

  value = DoSubstitution(value);

  return IntegerFromString(value);
}

size_t RymlDocumentNode::GetInteger(const std::string &name, size_t default_value)
{
  OTEL_INTERNAL_LOG_DEBUG("RymlDocumentNode::GetInteger(" << name << ", " << default_value << ")");

  auto ryml_child = GetRymlChildNode(name);

  if (ryml_child.invalid())
  {
    return default_value;
  }

  ryml::csubstr view = ryml_child.val();
  std::string value(view.str, view.len);

  value = DoSubstitution(value);

  if (value.empty())
  {
    return default_value;
  }

  return IntegerFromString(value);
}

double RymlDocumentNode::GetRequiredDouble(const std::string &name)
{
  OTEL_INTERNAL_LOG_DEBUG("RymlDocumentNode::GetRequiredDouble(" << name << ")");

  auto ryml_child = GetRequiredRymlChildNode(name);

  ryml::csubstr view = ryml_child.val();
  std::string value(view.str, view.len);

  value = DoSubstitution(value);

  return DoubleFromString(value);
}

double RymlDocumentNode::GetDouble(const std::string &name, double default_value)
{
  OTEL_INTERNAL_LOG_DEBUG("RymlDocumentNode::GetDouble(" << name << ", " << default_value << ")");

  auto ryml_child = GetRymlChildNode(name);

  if (ryml_child.invalid())
  {
    return default_value;
  }

  ryml::csubstr view = ryml_child.val();
  std::string value(view.str, view.len);

  value = DoSubstitution(value);

  if (value.empty())
  {
    return default_value;
  }

  return DoubleFromString(value);
}

std::string RymlDocumentNode::GetRequiredString(const std::string &name)
{
  OTEL_INTERNAL_LOG_DEBUG("RymlDocumentNode::GetRequiredString(" << name << ")");

  ryml::ConstNodeRef ryml_child = GetRequiredRymlChildNode(name);
  ryml::csubstr view            = ryml_child.val();
  std::string value(view.str, view.len);

  value = DoSubstitution(value);

  if (value.empty())
  {
    OTEL_INTERNAL_LOG_ERROR("Yaml: string value is empty: " << name);
    throw InvalidSchemaException(name);
  }

  return value;
}

std::string RymlDocumentNode::GetString(const std::string &name, const std::string &default_value)
{
  OTEL_INTERNAL_LOG_DEBUG("RymlDocumentNode::GetString(" << name << ", " << default_value << ")");

  ryml::ConstNodeRef ryml_child = GetRymlChildNode(name);

  if (ryml_child.invalid())
  {
    return default_value;
  }

  ryml::csubstr view = ryml_child.val();
  std::string value(view.str, view.len);

  value = DoSubstitution(value);

  return value;
}

DocumentNodeConstIterator RymlDocumentNode::begin() const
{
  OTEL_INTERNAL_LOG_DEBUG("RymlDocumentNode::begin()");

#ifdef WITH_DEBUG_NODE
  DebugNode("::begin()", node_);

  for (int index = 0; index < node_.num_children(); index++)
  {
    DebugNode("(child)", node_[index]);
  }
#endif  // WITH_DEBUG_NODE

  auto impl = std::unique_ptr<DocumentNodeConstIteratorImpl>(
      new RymlDocumentNodeConstIteratorImpl(node_, 0, depth_));

  return DocumentNodeConstIterator(std::move(impl));
}

DocumentNodeConstIterator RymlDocumentNode::end() const
{
  OTEL_INTERNAL_LOG_DEBUG("RymlDocumentNode::end()");

  auto impl = std::unique_ptr<DocumentNodeConstIteratorImpl>(
      new RymlDocumentNodeConstIteratorImpl(node_, node_.num_children(), depth_));

  return DocumentNodeConstIterator(std::move(impl));
}

size_t RymlDocumentNode::num_children() const
{
  return node_.num_children();
}

std::unique_ptr<DocumentNode> RymlDocumentNode::GetChild(size_t index) const
{
  std::unique_ptr<DocumentNode> child;
  ryml::ConstNodeRef ryml_child = node_[index];
  child = std::unique_ptr<DocumentNode>(new RymlDocumentNode(ryml_child, depth_ + 1));
  return child;
}

PropertiesNodeConstIterator RymlDocumentNode::begin_properties() const
{
  OTEL_INTERNAL_LOG_DEBUG("RymlDocumentNode::begin_properties()");

#ifdef WITH_DEBUG_NODE
  DebugNode("::begin_properties()", node_);

  for (int index = 0; index < node_.num_children(); index++)
  {
    DebugNode("(child)", node_[index]);
  }
#endif  // WITH_DEBUG_NODE

  auto impl = std::unique_ptr<PropertiesNodeConstIteratorImpl>(
      new RymlPropertiesNodeConstIteratorImpl(node_, 0, depth_));

  return PropertiesNodeConstIterator(std::move(impl));
}

PropertiesNodeConstIterator RymlDocumentNode::end_properties() const
{
  OTEL_INTERNAL_LOG_DEBUG("RymlDocumentNode::end_properties()");

  auto impl = std::unique_ptr<PropertiesNodeConstIteratorImpl>(
      new RymlPropertiesNodeConstIteratorImpl(node_, node_.num_children(), depth_));

  return PropertiesNodeConstIterator(std::move(impl));
}

RymlDocumentNodeConstIteratorImpl::RymlDocumentNodeConstIteratorImpl(ryml::ConstNodeRef parent,
                                                                     size_t index,
                                                                     size_t depth)
    : parent_(parent), index_(index), depth_(depth)
{}

RymlDocumentNodeConstIteratorImpl::~RymlDocumentNodeConstIteratorImpl() {}

void RymlDocumentNodeConstIteratorImpl::Next()
{
  ++index_;
}

std::unique_ptr<DocumentNode> RymlDocumentNodeConstIteratorImpl::Item() const
{
  std::unique_ptr<DocumentNode> item;
  ryml::ConstNodeRef ryml_item = parent_[index_];
  if (ryml_item.invalid())
  {
    OTEL_INTERNAL_LOG_ERROR("iterator is lost ");
    // Throw
  }
  item = std::unique_ptr<DocumentNode>(new RymlDocumentNode(ryml_item, depth_ + 1));
  return item;
}

bool RymlDocumentNodeConstIteratorImpl::Equal(const DocumentNodeConstIteratorImpl *rhs) const
{
  const RymlDocumentNodeConstIteratorImpl *other =
      static_cast<const RymlDocumentNodeConstIteratorImpl *>(rhs);
  return index_ == other->index_;
}

RymlPropertiesNodeConstIteratorImpl::RymlPropertiesNodeConstIteratorImpl(ryml::ConstNodeRef parent,
                                                                         size_t index,
                                                                         size_t depth)
    : parent_(parent), index_(index), depth_(depth)
{}

RymlPropertiesNodeConstIteratorImpl::~RymlPropertiesNodeConstIteratorImpl() {}

void RymlPropertiesNodeConstIteratorImpl::Next()
{
  OTEL_INTERNAL_LOG_DEBUG("RymlPropertiesNodeConstIteratorImpl::Next()");
  ++index_;
}

std::string RymlPropertiesNodeConstIteratorImpl::Name() const
{
  ryml::ConstNodeRef ryml_item = parent_[index_];
  // FIXME: check there is a key()
  ryml::csubstr k = ryml_item.key();
  std::string name(k.str, k.len);

  OTEL_INTERNAL_LOG_DEBUG("RymlPropertiesNodeConstIteratorImpl::Name() = " << name);

  return name;
}

std::unique_ptr<DocumentNode> RymlPropertiesNodeConstIteratorImpl::Value() const
{
  std::unique_ptr<DocumentNode> item;

  ryml::ConstNodeRef ryml_item = parent_[index_];
  item = std::unique_ptr<DocumentNode>(new RymlDocumentNode(ryml_item, depth_ + 1));

  OTEL_INTERNAL_LOG_DEBUG("RymlPropertiesNodeConstIteratorImpl::Value()");

  return item;
}

bool RymlPropertiesNodeConstIteratorImpl::Equal(const PropertiesNodeConstIteratorImpl *rhs) const
{
  const RymlPropertiesNodeConstIteratorImpl *other =
      static_cast<const RymlPropertiesNodeConstIteratorImpl *>(rhs);
  return index_ == other->index_;
}

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
