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
  DocumentNode()          = default;
  virtual ~DocumentNode() = default;

  virtual bool AsBoolean()       = 0;
  virtual size_t AsInteger()     = 0;
  virtual double AsDouble()      = 0;
  virtual std::string AsString() = 0;

  virtual std::unique_ptr<DocumentNode> GetRequiredChildNode(const std::string &name) = 0;
  virtual std::unique_ptr<DocumentNode> GetChildNode(const std::string &name)         = 0;

  virtual bool GetRequiredBoolean(const std::string &name)             = 0;
  virtual bool GetBoolean(const std::string &name, bool default_value) = 0;

  virtual size_t GetRequiredInteger(const std::string &name)               = 0;
  virtual size_t GetInteger(const std::string &name, size_t default_value) = 0;

  virtual double GetRequiredDouble(const std::string &name)               = 0;
  virtual double GetDouble(const std::string &name, double default_value) = 0;

  virtual std::string GetRequiredString(const std::string &name)                           = 0;
  virtual std::string GetString(const std::string &name, const std::string &default_value) = 0;

  virtual DocumentNodeConstIterator begin() const = 0;
  virtual DocumentNodeConstIterator end() const   = 0;

  virtual size_t num_children() const                                = 0;
  virtual std::unique_ptr<DocumentNode> GetChild(size_t index) const = 0;

  virtual PropertiesNodeConstIterator begin_properties() const = 0;
  virtual PropertiesNodeConstIterator end_properties() const   = 0;

  virtual std::string Dump() const = 0;

protected:
  void DoSubstitution(std::string &value);

  bool BooleanFromString(const std::string &value);
  size_t IntegerFromString(const std::string &value);
  double DoubleFromString(const std::string &value);
};

class DocumentNodeConstIteratorImpl
{
public:
  DocumentNodeConstIteratorImpl()          = default;
  virtual ~DocumentNodeConstIteratorImpl() = default;

  virtual void Next()                                                = 0;
  virtual std::unique_ptr<DocumentNode> Item() const                 = 0;
  virtual bool Equal(const DocumentNodeConstIteratorImpl *rhs) const = 0;
};

class PropertiesNodeConstIteratorImpl
{
public:
  PropertiesNodeConstIteratorImpl()          = default;
  virtual ~PropertiesNodeConstIteratorImpl() = default;

  virtual void Next()                                                  = 0;
  virtual std::string Name() const                                     = 0;
  virtual std::unique_ptr<DocumentNode> Value() const                  = 0;
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
