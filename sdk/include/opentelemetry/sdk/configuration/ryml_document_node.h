// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <stddef.h>
#include <memory>
#include <ryml.hpp>
#include <string>

#include "opentelemetry/sdk/configuration/document_node.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

class RymlDocumentNode : public DocumentNode
{
public:
  RymlDocumentNode(ryml::ConstNodeRef node, std::size_t depth) : node_(node), depth_(depth) {}
  RymlDocumentNode(RymlDocumentNode &&)                      = delete;
  RymlDocumentNode(const RymlDocumentNode &)                 = delete;
  RymlDocumentNode &operator=(RymlDocumentNode &&)           = delete;
  RymlDocumentNode &operator=(const RymlDocumentNode &other) = delete;
  ~RymlDocumentNode() override                               = default;

  std::string Key() const override;

  bool AsBoolean() override;
  std::size_t AsInteger() override;
  double AsDouble() override;
  std::string AsString() override;

  std::unique_ptr<DocumentNode> GetRequiredChildNode(const std::string &name) override;
  std::unique_ptr<DocumentNode> GetChildNode(const std::string &name) override;

  bool GetRequiredBoolean(const std::string &name) override;
  bool GetBoolean(const std::string &name, bool default_value) override;

  std::size_t GetRequiredInteger(const std::string &name) override;
  std::size_t GetInteger(const std::string &name, std::size_t default_value) override;

  double GetRequiredDouble(const std::string &name) override;
  double GetDouble(const std::string &name, double default_value) override;

  std::string GetRequiredString(const std::string &name) override;
  std::string GetString(const std::string &name, const std::string &default_value) override;

  DocumentNodeConstIterator begin() const override;
  DocumentNodeConstIterator end() const override;

  std::size_t num_children() const override;
  std::unique_ptr<DocumentNode> GetChild(std::size_t index) const override;

  PropertiesNodeConstIterator begin_properties() const override;
  PropertiesNodeConstIterator end_properties() const override;

private:
  ryml::ConstNodeRef GetRequiredRymlChildNode(const std::string &name);
  ryml::ConstNodeRef GetRymlChildNode(const std::string &name);

  ryml::ConstNodeRef node_;
  std::size_t depth_;
};

class RymlDocumentNodeConstIteratorImpl : public DocumentNodeConstIteratorImpl
{
public:
  RymlDocumentNodeConstIteratorImpl(ryml::ConstNodeRef parent,
                                    std::size_t index,
                                    std::size_t depth);
  RymlDocumentNodeConstIteratorImpl(RymlDocumentNodeConstIteratorImpl &&)            = delete;
  RymlDocumentNodeConstIteratorImpl(const RymlDocumentNodeConstIteratorImpl &)       = delete;
  RymlDocumentNodeConstIteratorImpl &operator=(RymlDocumentNodeConstIteratorImpl &&) = delete;
  RymlDocumentNodeConstIteratorImpl &operator=(const RymlDocumentNodeConstIteratorImpl &other) =
      delete;
  ~RymlDocumentNodeConstIteratorImpl() override;

  void Next() override;
  std::unique_ptr<DocumentNode> Item() const override;
  bool Equal(const DocumentNodeConstIteratorImpl *rhs) const override;

private:
  ryml::ConstNodeRef parent_;
  std::size_t index_;
  std::size_t depth_;
};

class RymlPropertiesNodeConstIteratorImpl : public PropertiesNodeConstIteratorImpl
{
public:
  RymlPropertiesNodeConstIteratorImpl(ryml::ConstNodeRef parent,
                                      std::size_t index,
                                      std::size_t depth);
  RymlPropertiesNodeConstIteratorImpl(RymlPropertiesNodeConstIteratorImpl &&)            = delete;
  RymlPropertiesNodeConstIteratorImpl(const RymlPropertiesNodeConstIteratorImpl &)       = delete;
  RymlPropertiesNodeConstIteratorImpl &operator=(RymlPropertiesNodeConstIteratorImpl &&) = delete;
  RymlPropertiesNodeConstIteratorImpl &operator=(const RymlPropertiesNodeConstIteratorImpl &other) =
      delete;
  ~RymlPropertiesNodeConstIteratorImpl() override;

  void Next() override;
  std::string Name() const override;
  std::unique_ptr<DocumentNode> Value() const override;
  bool Equal(const PropertiesNodeConstIteratorImpl *rhs) const override;

private:
  ryml::ConstNodeRef parent_;
  std::size_t index_;
  std::size_t depth_;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
