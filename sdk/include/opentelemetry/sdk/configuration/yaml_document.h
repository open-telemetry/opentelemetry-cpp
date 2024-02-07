// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yaml-cpp/yaml.h>
#include <string>

#include "opentelemetry/sdk/configuration/document.h"
#include "opentelemetry/sdk/configuration/document_node.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

class YamlDocument : public Document
{
public:
  static std::unique_ptr<Document> Parse(std::istream &in);

  YamlDocument(YAML::Node root) : m_root(root) {}
  ~YamlDocument() override = default;

  std::unique_ptr<DocumentNode> GetRootNode() override;

private:
  YAML::Node m_root;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
