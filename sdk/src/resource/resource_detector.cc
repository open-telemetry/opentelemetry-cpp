// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/resource/resource_detector.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/sdk/common/env_variables.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/semconv/service_attributes.h"
#include "opentelemetry/version.h"

#include <cctype>
#include <cstddef>
#include <sstream>
#include <string>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace resource
{

constexpr const char *kOtelResourceAttributes = "OTEL_RESOURCE_ATTRIBUTES";
constexpr const char *kOtelServiceName        = "OTEL_SERVICE_NAME";

namespace
{
unsigned char HexDigitValue(unsigned char c) noexcept
{
  return static_cast<unsigned char>(std::isdigit(c) ? c - '0' : std::toupper(c) - 'A' + 10);
}

// Percent-decodes a value from OTEL_RESOURCE_ATTRIBUTES, per the W3C Baggage value grammar
// the resource spec defers to
// (https://github.com/open-telemetry/opentelemetry-specification/pull/2670). A malformed escape
// sequence is left in the output as-is rather than dropping the attribute, since this parser has
// never validated attribute syntax.
std::string PercentDecode(const std::string &value)
{
  std::string decoded;
  decoded.reserve(value.size());
  for (std::size_t i = 0; i < value.size(); ++i)
  {
    if (value[i] == '%' && i + 2 < value.size())
    {
      unsigned char high = static_cast<unsigned char>(value[i + 1]);
      unsigned char low  = static_cast<unsigned char>(value[i + 2]);
      if (std::isxdigit(high) && std::isxdigit(low))
      {
        unsigned char unescaped_value =
            static_cast<unsigned char>((HexDigitValue(high) << 4) | HexDigitValue(low));
        decoded.push_back(static_cast<char>(unescaped_value));
        i += 2;
        continue;
      }
    }
    decoded.push_back(value[i]);
  }
  return decoded;
}
}  // namespace

Resource ResourceDetector::Create(const ResourceAttributes &attributes,
                                  const std::string &schema_url)
{
  return Resource(attributes, schema_url);
}

Resource OTELResourceDetector::Detect() noexcept
{
  std::string attributes_str, service_name;

  bool attributes_exists = opentelemetry::sdk::common::GetStringEnvironmentVariable(
      kOtelResourceAttributes, attributes_str);
  bool service_name_exists =
      opentelemetry::sdk::common::GetStringEnvironmentVariable(kOtelServiceName, service_name);

  if (!attributes_exists && !service_name_exists)
  {
    return ResourceDetector::Create({});
  }

  ResourceAttributes attributes;

  if (attributes_exists)
  {
    std::istringstream iss(attributes_str);
    std::string token;
    while (std::getline(iss, token, ','))
    {
      size_t pos = token.find('=');
      if (pos != std::string::npos)
      {
        std::string key   = token.substr(0, pos);
        std::string value = token.substr(pos + 1);
        attributes[key]   = PercentDecode(value);
      }
    }
  }

  if (service_name_exists)
  {
    attributes[semconv::service::kServiceName] = service_name;
  }

  return ResourceDetector::Create(attributes);
}

}  // namespace resource
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
