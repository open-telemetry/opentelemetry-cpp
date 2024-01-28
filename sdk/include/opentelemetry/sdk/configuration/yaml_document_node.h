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

class YamlDocumentNode : public DocumentNode
{
public:
  YamlDocumentNode(YAML::Node yaml) : m_yaml(yaml) {}
  ~YamlDocumentNode() override = default;

  bool AsBoolean() override;
  size_t AsInteger() override;
  double AsDouble() override;
  std::string AsString() override;

  std::unique_ptr<DocumentNode> GetRequiredChildNode(std::string_view name) override;
  std::unique_ptr<DocumentNode> GetChildNode(std::string_view name) override;

  bool GetRequiredBoolean(std::string_view name) override;
  bool GetBoolean(std::string_view name, bool default_value) override;

  size_t GetRequiredInteger(std::string_view name) override;
  size_t GetInteger(std::string_view name, size_t default_value) override;

  double GetRequiredDouble(std::string_view name) override;
  double GetDouble(std::string_view name, double default_value) override;

  std::string GetRequiredString(std::string_view name) override;
  std::string GetString(std::string_view name, std::string_view default_value) override;

  DocumentNodeConstIterator begin() const override;
  DocumentNodeConstIterator end() const override;

  PropertiesNodeConstIterator begin_properties() const override;
  PropertiesNodeConstIterator end_properties() const override;

  std::string Dump() const override;

private:
  YAML::Node m_yaml;
};

class YamlDocumentNodeConstIteratorImpl : public DocumentNodeConstIteratorImpl
{
public:
  YamlDocumentNodeConstIteratorImpl(const YAML::const_iterator &iter);
  virtual ~YamlDocumentNodeConstIteratorImpl();

  void Next() override;
  std::unique_ptr<DocumentNode> Item() const override;
  bool Equal(const DocumentNodeConstIteratorImpl *rhs) const override;

private:
  YAML::const_iterator m_iter;
};

class YamlPropertiesNodeConstIteratorImpl : public PropertiesNodeConstIteratorImpl
{
public:
  YamlPropertiesNodeConstIteratorImpl(const YAML::const_iterator &iter);
  virtual ~YamlPropertiesNodeConstIteratorImpl();

  void Next() override;
  std::string Name() const override;
  std::unique_ptr<DocumentNode> Value() const override;
  bool Equal(const PropertiesNodeConstIteratorImpl *rhs) const override;

private:
  YAML::const_iterator m_iter;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
