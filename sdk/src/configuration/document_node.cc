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

/**
  Perform substitution on a string scalar.
  Only if the entire scalar string is: ${ENV_NAME},
  this code does not perform substitution in:
  "some text with ${ENV_NAME} in it".
*/
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

  for (size_t i = 3; i + 2 <= len; i++)
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

  OTEL_INTERNAL_LOG_ERROR("Illegal integer value: " << value);
  throw InvalidSchemaException("Illegal bool value");
}

size_t DocumentNode::IntegerFromString(const std::string &value)
{
  const char *ptr = value.c_str();
  char *end       = nullptr;
  int len         = value.length();
  size_t val      = strtoll(ptr, &end, 10);
  if (ptr + len != end)
  {
    OTEL_INTERNAL_LOG_ERROR("Illegal integer value: " << value);
    throw InvalidSchemaException("Illegal integer value");
  }
  return val;
}

double DocumentNode::DoubleFromString(const std::string &value)
{
  const char *ptr = value.c_str();
  char *end       = nullptr;
  int len         = value.length();
  double val      = strtod(ptr, &end);
  if (ptr + len != end)
  {
    OTEL_INTERNAL_LOG_ERROR("Illegal double value: " << value);
    throw InvalidSchemaException("Illegal double value");
  }
  return val;
}

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
