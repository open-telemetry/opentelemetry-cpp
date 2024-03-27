// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <ryml.hpp>
#include <string>

#include "opentelemetry/sdk/configuration/document.h"
#include "opentelemetry/sdk/configuration/document_node.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

class RymlDocument : public Document
{
public:
  static std::unique_ptr<Document> Parse(std::string content);

  RymlDocument(ryml::Tree tree) : m_tree(tree) {}
  ~RymlDocument() override = default;

  std::unique_ptr<DocumentNode> GetRootNode() override;

private:
  ryml::Tree m_tree;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
