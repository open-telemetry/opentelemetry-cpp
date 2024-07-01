// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <ryml.hpp>
#include <ryml_std.hpp>

#include "opentelemetry/sdk/common/global_log_handler.h"

#include "opentelemetry/sdk/configuration/ryml_document.h"
#include "opentelemetry/sdk/configuration/ryml_document_node.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

std::unique_ptr<Document> RymlDocument::Parse(std::string content)
{
  ryml::Tree tree;
  std::unique_ptr<Document> doc;

  try
  {
    tree = ryml::parse_in_arena(ryml::to_csubstr(content));
    tree.resolve();
  }
  catch (...)
  {
    OTEL_INTERNAL_LOG_ERROR("Failed to load yaml.");
    return doc;
  }

  RymlDocument *ryml_doc = new RymlDocument(tree);
  doc                    = std::unique_ptr<Document>(ryml_doc);
  return doc;
}

std::unique_ptr<DocumentNode> RymlDocument::GetRootNode()
{
  RymlDocumentNode *ryml_node = new RymlDocumentNode(m_tree.rootref());
  std::unique_ptr<DocumentNode> node(ryml_node);
  return node;
}

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
