// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>

#include "opentelemetry/sdk/configuration/attribute_limit_configuration.h"
#include "opentelemetry/sdk/configuration/logger_provider_configuration.h"
#include "opentelemetry/sdk/configuration/meter_provider_configuration.h"
#include "opentelemetry/sdk/configuration/propagator_configuration.h"
#include "opentelemetry/sdk/configuration/resource_configuration.h"
#include "opentelemetry/sdk/configuration/tracer_provider_configuration.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

class DocumentNodeConstIterator;
class PropertiesNodeConstIterator;

class DocumentNode
{
public:
  DocumentNode() {}
  virtual ~DocumentNode() {}

  virtual bool AsBoolean() = 0;
  virtual size_t AsInteger() = 0;
  virtual std::string AsString() = 0;

  virtual std::unique_ptr<DocumentNode> GetRequiredChildNode(std::string_view name) = 0;
  virtual std::unique_ptr<DocumentNode> GetChildNode(std::string_view name)         = 0;

  virtual bool GetRequiredBoolean(std::string_view name)             = 0;
  virtual bool GetBoolean(std::string_view name, bool default_value) = 0;

  virtual size_t GetRequiredInteger(std::string_view name)               = 0;
  virtual size_t GetInteger(std::string_view name, size_t default_value) = 0;

  virtual std::string GetRequiredString(std::string_view name)                         = 0;
  virtual std::string GetString(std::string_view name, std::string_view default_value) = 0;

  virtual DocumentNodeConstIterator begin() const = 0;
  virtual DocumentNodeConstIterator end() const   = 0;

  virtual PropertiesNodeConstIterator begin_properties() const = 0;
  virtual PropertiesNodeConstIterator end_properties() const   = 0;

  virtual std::string Dump() const = 0;
};

class DocumentNodeConstIteratorImpl
{
public:
  DocumentNodeConstIteratorImpl() {}
  virtual ~DocumentNodeConstIteratorImpl() {}

  virtual void Next()                                                = 0;
  virtual std::unique_ptr<DocumentNode> Item() const                 = 0;
  virtual bool Equal(const DocumentNodeConstIteratorImpl *rhs) const = 0;
};

class PropertiesNodeConstIteratorImpl
{
public:
  PropertiesNodeConstIteratorImpl() {}
  virtual ~PropertiesNodeConstIteratorImpl() {}

  virtual void Next()                                                = 0;
  virtual std::string Name() const                 = 0;
  virtual std::unique_ptr<DocumentNode> Value() const                 = 0;
  virtual bool Equal(const PropertiesNodeConstIteratorImpl *rhs) const = 0;
};

class DocumentNodeConstIterator
{
public:
  DocumentNodeConstIterator(DocumentNodeConstIteratorImpl *impl) : m_impl(impl) {}
  ~DocumentNodeConstIterator() { delete m_impl; }

  bool operator==(const DocumentNodeConstIterator &rhs) const
  {
    return (m_impl->Equal(rhs.m_impl));
  }

  bool operator!=(const DocumentNodeConstIterator &rhs) const
  {
    return (!m_impl->Equal(rhs.m_impl));
  }

  std::unique_ptr<DocumentNode> operator*() const { return m_impl->Item(); }

  DocumentNodeConstIterator &operator++()
  {
    m_impl->Next();
    return *this;
  }

#ifdef NEVER
  DocumentNodeConstIterator &operator++(int)
  {
    m_impl->Next();
    return *this;
  }
#endif

private:
  DocumentNodeConstIteratorImpl *m_impl;
};

class PropertiesNodeConstIterator
{
public:
  PropertiesNodeConstIterator(PropertiesNodeConstIteratorImpl *impl) : m_impl(impl) {}
  ~PropertiesNodeConstIterator() { delete m_impl; }

  bool operator==(const PropertiesNodeConstIterator &rhs) const
  {
    return (m_impl->Equal(rhs.m_impl));
  }

  bool operator!=(const PropertiesNodeConstIterator &rhs) const
  {
    return (!m_impl->Equal(rhs.m_impl));
  }

  std::string Name() const { return m_impl->Name(); }
  std::unique_ptr<DocumentNode> Value() const { return m_impl->Value(); }

  PropertiesNodeConstIterator &operator++()
  {
    m_impl->Next();
    return *this;
  }

private:
  PropertiesNodeConstIteratorImpl *m_impl;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
