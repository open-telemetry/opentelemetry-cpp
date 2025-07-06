// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <cctype>
#include <cstdlib>
#include <ostream>
#include <string>

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
 * Perform environment variables substitution on a full line.
 * Supported:
 * - Text line with no substitution
 * - ${SUBSTITUTION}
 * - Some text with ${SUBSTITUTION} in it
 * - Multiple ${SUBSTITUTION_A} substitutions ${SUBSTITUTION_B} in the line
 */
std::string DocumentNode::DoSubstitution(const std::string &text)
{
  static std::string begin_token{"${"};
  static std::string end_token{"}"};

  std::string result;

  std::string::size_type current_pos;
  std::string::size_type begin_pos;
  std::string::size_type end_pos;
  std::string copy;
  std::string substitution;

  current_pos = 0;
  begin_pos   = text.find(begin_token);

  while (begin_pos != std::string::npos)
  {
    if (current_pos < begin_pos)
    {
      /*
       * ^ < COPY_ME > ${...
       * ...} < COPY_ME > ${...
       *
       * copy [current_pos, begin_pos[
       */
      copy = text.substr(current_pos, begin_pos - current_pos);
      result.append(copy);
      current_pos = begin_pos;
    }

    end_pos = text.find(end_token, begin_pos);

    if (end_pos == std::string::npos)
    {
      /* ... < ${NOT_A_SUBSTITUTION > */
      copy = text.substr(current_pos);
      result.append(copy);
      return result;
    }

    /* ... < ${SUBSTITUTION} > ... */
    substitution = text.substr(current_pos, end_pos - current_pos + 1);

    copy = DoOneSubstitution(substitution);
    result.append(copy);
    begin_pos   = text.find(begin_token, end_pos);
    current_pos = end_pos + 1;
  }

  copy = text.substr(current_pos);
  result.append(copy);

  return result;
}

/**
  Perform one substitution on a string scalar.
  Supported:
  - ${ENV_NAME}
  - ${env:ENV_NAME}
  - ${ENV_NAME:-fallback} (including when ENV_NAME is actually "env")
  - ${env:ENV_NAME:-fallback}
*/
std::string DocumentNode::DoOneSubstitution(const std::string &text)
{
  static std::string illegal_msg{"Illegal substitution expression"};

  static std::string env_token{"env:"};
  static std::string env_with_replacement{"env:-"};
  static std::string replacement_token{":-"};

  size_t len = text.length();
  char c;
  std::string::size_type begin_name;
  std::string::size_type end_name;
  std::string::size_type begin_fallback;
  std::string::size_type end_fallback;
  std::string::size_type pos;
  std::string name;
  std::string fallback;
  const char *sub;

  if (len < 4)
  {
    OTEL_INTERNAL_LOG_ERROR(illegal_msg << ": " << text);
    throw InvalidSchemaException(illegal_msg);
  }

  c = text[0];
  if (c != '$')
  {
    OTEL_INTERNAL_LOG_ERROR(illegal_msg << ": " << text);
    throw InvalidSchemaException(illegal_msg);
  }

  c = text[1];
  if (c != '{')
  {
    OTEL_INTERNAL_LOG_ERROR(illegal_msg << ": " << text);
    throw InvalidSchemaException(illegal_msg);
  }

  c = text[len - 1];
  if (c != '}')
  {
    OTEL_INTERNAL_LOG_ERROR(illegal_msg << ": " << text);
    throw InvalidSchemaException(illegal_msg);
  }

  begin_name = 2;

  /* If text[2] starts with "env:" */
  if (text.find(env_token, begin_name) == begin_name)
  {
    /* If text[2] starts with "env:-" */
    if (text.find(env_with_replacement, begin_name) == begin_name)
    {
      /* ${env:-fallback} is legal. It is variable "env". */
    }
    else
    {
      begin_name += env_token.length();  // Skip "env:"
    }
  }

  c = text[begin_name];
  if (!std::isalpha(c) && c != '_')
  {
    OTEL_INTERNAL_LOG_ERROR(illegal_msg << ": " << text);
    throw InvalidSchemaException(illegal_msg);
  }

  end_name = begin_name + 1;

  for (size_t i = begin_name + 1; i + 2 <= len; i++)
  {
    c = text[i];
    if (std::isalnum(c) || (c == '_'))
    {
      end_name = i + 1;
    }
    else
    {
      break;
    }
  }

  // text is of the form ${ENV_NAME ...

  name = text.substr(begin_name, end_name - begin_name);

  if (end_name + 1 == len)
  {
    // text is of the form ${ENV_NAME}
    begin_fallback = 0;
    end_fallback   = 0;
  }
  else
  {
    pos = text.find(replacement_token, end_name);
    if (pos != end_name)
    {
      OTEL_INTERNAL_LOG_ERROR(illegal_msg << ": " << text);
      throw InvalidSchemaException(illegal_msg);
    }
    // text is of the form ${ENV_NAME:-fallback}
    begin_fallback = pos + replacement_token.length();
    end_fallback   = len - 1;
  }

  sub = std::getenv(name.c_str());
  if (sub != nullptr)
  {
    return sub;
  }

  if (begin_fallback < end_fallback)
  {
    fallback = text.substr(begin_fallback, end_fallback - begin_fallback);
  }
  else
  {
    fallback = "";
  }
  return fallback;
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
  size_t len      = value.length();
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
  size_t len      = value.length();
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
