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
  static constexpr std::size_t MAX_NODE_DEPTH = 100;

  DocumentNode()                                     = default;
  DocumentNode(DocumentNode &&)                      = default;
  DocumentNode(const DocumentNode &)                 = default;
  DocumentNode &operator=(DocumentNode &&)           = default;
  DocumentNode &operator=(const DocumentNode &other) = default;
  virtual ~DocumentNode()                            = default;

  virtual std::string Key() const = 0;

  virtual bool AsBoolean()        = 0;
  virtual std::size_t AsInteger() = 0;
  virtual double AsDouble()       = 0;
  virtual std::string AsString()  = 0;

  virtual std::unique_ptr<DocumentNode> GetRequiredChildNode(const std::string &name) = 0;
  virtual std::unique_ptr<DocumentNode> GetChildNode(const std::string &name)         = 0;

  virtual bool GetRequiredBoolean(const std::string &name)             = 0;
  virtual bool GetBoolean(const std::string &name, bool default_value) = 0;

  virtual std::size_t GetRequiredInteger(const std::string &name)                    = 0;
  virtual std::size_t GetInteger(const std::string &name, std::size_t default_value) = 0;

  virtual double GetRequiredDouble(const std::string &name)               = 0;
  virtual double GetDouble(const std::string &name, double default_value) = 0;

  virtual std::string GetRequiredString(const std::string &name)                           = 0;
  virtual std::string GetString(const std::string &name, const std::string &default_value) = 0;

  virtual DocumentNodeConstIterator begin() const = 0;
  virtual DocumentNodeConstIterator end() const   = 0;

  virtual std::size_t num_children() const                                = 0;
  virtual std::unique_ptr<DocumentNode> GetChild(std::size_t index) const = 0;

  virtual PropertiesNodeConstIterator begin_properties() const = 0;
  virtual PropertiesNodeConstIterator end_properties() const   = 0;

  virtual std::string Dump() const = 0;

protected:
  std::string DoSubstitution(const std::string &text);
  std::string DoOneSubstitution(const std::string &text);

  bool BooleanFromString(const std::string &value);
  std::size_t IntegerFromString(const std::string &value);
  double DoubleFromString(const std::string &value);
};

class DocumentNodeConstIteratorImpl
{
public:
  DocumentNodeConstIteratorImpl()                                                      = default;
  DocumentNodeConstIteratorImpl(DocumentNodeConstIteratorImpl &&)                      = default;
  DocumentNodeConstIteratorImpl(const DocumentNodeConstIteratorImpl &)                 = default;
  DocumentNodeConstIteratorImpl &operator=(DocumentNodeConstIteratorImpl &&)           = default;
  DocumentNodeConstIteratorImpl &operator=(const DocumentNodeConstIteratorImpl &other) = default;
  virtual ~DocumentNodeConstIteratorImpl()                                             = default;

  virtual void Next()                                                = 0;
  virtual std::unique_ptr<DocumentNode> Item() const                 = 0;
  virtual bool Equal(const DocumentNodeConstIteratorImpl *rhs) const = 0;
};

class PropertiesNodeConstIteratorImpl
{
public:
  PropertiesNodeConstIteratorImpl()                                              = default;
  PropertiesNodeConstIteratorImpl(PropertiesNodeConstIteratorImpl &&)            = default;
  PropertiesNodeConstIteratorImpl(const PropertiesNodeConstIteratorImpl &)       = default;
  PropertiesNodeConstIteratorImpl &operator=(PropertiesNodeConstIteratorImpl &&) = default;
  PropertiesNodeConstIteratorImpl &operator=(const PropertiesNodeConstIteratorImpl &other) =
      default;
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
  DocumentNodeConstIterator(DocumentNodeConstIterator &&)                      = default;
  DocumentNodeConstIterator(const DocumentNodeConstIterator &)                 = default;
  DocumentNodeConstIterator &operator=(DocumentNodeConstIterator &&)           = default;
  DocumentNodeConstIterator &operator=(const DocumentNodeConstIterator &other) = default;

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
  PropertiesNodeConstIterator(PropertiesNodeConstIterator &&)                      = default;
  PropertiesNodeConstIterator(const PropertiesNodeConstIterator &)                 = default;
  PropertiesNodeConstIterator &operator=(PropertiesNodeConstIterator &&)           = default;
  PropertiesNodeConstIterator &operator=(const PropertiesNodeConstIterator &other) = default;
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
