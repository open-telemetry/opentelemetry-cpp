// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/common/global_log_handler.h"

#include "opentelemetry/sdk/configuration/document_node.h"
#include "opentelemetry/sdk/configuration/invalid_schema_exception.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

void DocumentNode::DoSubstitution(std::string &value)
{
  size_t len = value.length();
  char c;

  if (len < 4)
  {
    return;
  }

  c = value[0];
  if (c != '$')
  {
    return;
  }

  c = value[1];
  if (c != '{')
  {
    return;
  }

  c = value[len - 1];
  if (c != '}')
  {
    return;
  }

  c = value[2];
  if (!std::isalpha(c) && c != '_')
  {
    return;
  }

  for (int i = 3; i <= len - 2; i++)
  {
    c = value[i];
    if (!std::isalnum(c) && c != '_')
    {
      return;
    }
  }

  // value is of the form ${ENV_NAME}

  std::string name = value.substr(2, len - 3);

  const char *sub = std::getenv(name.c_str());
  if (sub != nullptr)
  {
    value = sub;
  }
  else
  {
    value = "";
  }
}

bool DocumentNode::BooleanFromString(const std::string &value)
{
  if (value == "true")
  {
    return true;
  }

  if (value == "false")
  {
    return false;
  }

  throw InvalidSchemaException("Illegal bool value");
}

size_t DocumentNode::IntegerFromString(const std::string &value)
{
  size_t val = atoll(value.c_str());
  return val;
}

double DocumentNode::DoubleFromString(const std::string &value)
{
  double val = atof(value.c_str());
  return val;
}

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
