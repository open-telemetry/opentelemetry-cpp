// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <exception>
#include <memory>
#include <ostream>
#include <ryml.hpp>
#include <ryml_std.hpp>
#include <string>

#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/configuration/document.h"
#include "opentelemetry/sdk/configuration/document_node.h"
#include "opentelemetry/sdk/configuration/ryml_document.h"
#include "opentelemetry/sdk/configuration/ryml_document_node.h"
#include "opentelemetry/version.h"

/* We require 0.7.1 or better */
#define OTEL_HAVE_RYML 7

/* Code using ryml 0.6.0 preserved for regression testing. */
/* #define OTEL_HAVE_RYML 6 */

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

std::unique_ptr<Document> RymlDocument::Parse(const std::string &source, const std::string &content)
{
  ryml::ParserOptions opts;
  opts.locations(true);

#if OTEL_HAVE_RYML == 6
  ryml::Parser parser(opts);
#endif

#if OTEL_HAVE_RYML == 7
  ryml::Parser::handler_type event_handler;
  ryml::Parser parser(&event_handler, opts);
#endif

  ryml::Tree tree;
  ryml::csubstr filename;
  ryml::csubstr csubstr_content;
  std::unique_ptr<Document> doc;

  filename        = ryml::to_csubstr(source);
  csubstr_content = ryml::to_csubstr(content);

  try
  {
#if OTEL_HAVE_RYML == 6
    tree = parser.parse_in_arena(filename, csubstr_content);
#endif

#if OTEL_HAVE_RYML == 7
    tree = parse_in_arena(&parser, filename, csubstr_content);
#endif

    tree.resolve();
  }
  catch (const std::exception &e)
  {
    OTEL_INTERNAL_LOG_ERROR("[Ryml Document] Parse failed with exception: " << e.what());
    return doc;
  }
  catch (...)
  {
    OTEL_INTERNAL_LOG_ERROR("[Ryml Document] Parse failed with unknown exception.");
    return doc;
  }

  RymlDocument *ryml_doc = new RymlDocument(tree);
  doc                    = std::unique_ptr<Document>(ryml_doc);
  return doc;
}

std::unique_ptr<DocumentNode> RymlDocument::GetRootNode()
{
  RymlDocumentNode *ryml_node = new RymlDocumentNode(tree_.rootref(), 0);
  std::unique_ptr<DocumentNode> node(ryml_node);
  return node;
}

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
