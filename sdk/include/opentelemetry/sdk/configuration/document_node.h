// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>
#include <string>

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
  // FIXME: proper sizing
  static constexpr size_t MAX_NODE_DEPTH = 100;

  DocumentNode()          = default;
  virtual ~DocumentNode() = default;

  virtual std::string Key() const = 0;

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
  std::string DoSubstitution(const std::string &text);
  std::string DoOneSubstitution(const std::string &text);

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
  DocumentNodeConstIterator(DocumentNodeConstIteratorImpl *impl) : impl_(impl) {}
  ~DocumentNodeConstIterator() { delete impl_; }

  bool operator==(const DocumentNodeConstIterator &rhs) const { return (impl_->Equal(rhs.impl_)); }

  bool operator!=(const DocumentNodeConstIterator &rhs) const { return (!impl_->Equal(rhs.impl_)); }

  std::unique_ptr<DocumentNode> operator*() const { return impl_->Item(); }

  DocumentNodeConstIterator &operator++()
  {
    impl_->Next();
    return *this;
  }

private:
  DocumentNodeConstIteratorImpl *impl_;
};

class PropertiesNodeConstIterator
{
public:
  PropertiesNodeConstIterator(PropertiesNodeConstIteratorImpl *impl) : impl_(impl) {}
  ~PropertiesNodeConstIterator() { delete impl_; }

  bool operator==(const PropertiesNodeConstIterator &rhs) const
  {
    return (impl_->Equal(rhs.impl_));
  }

  bool operator!=(const PropertiesNodeConstIterator &rhs) const
  {
    return (!impl_->Equal(rhs.impl_));
  }

  std::string Name() const { return impl_->Name(); }
  std::unique_ptr<DocumentNode> Value() const { return impl_->Value(); }

  PropertiesNodeConstIterator &operator++()
  {
    impl_->Next();
    return *this;
  }

private:
  PropertiesNodeConstIteratorImpl *impl_;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
