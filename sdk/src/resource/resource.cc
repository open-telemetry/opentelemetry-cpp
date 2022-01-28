// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/sdk/resource/experimental_semantic_conventions.h"
#include "opentelemetry/sdk/resource/resource_detector.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace resource
{

const std::string kTelemetrySdkLanguage  = "telemetry.sdk.language";
const std::string kTelemetrySdkName      = "telemetry.sdk.name";
const std::string kTelemetrySdkVersion   = "telemetry.sdk.version";
const std::string kServiceName           = "service.name";
const std::string kProcessExecutableName = "process.executable.name";

Resource::Resource(const ResourceAttributes &attributes, const std::string &schema_url) noexcept
    : attributes_(attributes), schema_url_(schema_url)
{}

Resource Resource::Merge(const Resource &other) noexcept
{
  ResourceAttributes merged_resource_attributes(other.attributes_);
  merged_resource_attributes.insert(attributes_.begin(), attributes_.end());
  return Resource(merged_resource_attributes, other.schema_url_);
}

Resource Resource::Create(const ResourceAttributes &attributes, const std::string &schema_url)
{
  static auto otel_resource = OTELResourceDetector().Detect();
  auto resource =
      Resource::GetDefault().Merge(otel_resource).Merge(Resource{attributes, schema_url});

  if (resource.attributes_.find(OTEL_GET_RESOURCE_ATTR(AttrServiceName)) ==
      resource.attributes_.end())
  {
    std::string default_service_name = "unknown_service";
    auto it_process_executable_name =
        resource.attributes_.find(OTEL_GET_RESOURCE_ATTR(AttrProcessExecutableName));
    if (it_process_executable_name != resource.attributes_.end())
    {
      default_service_name += ":" + nostd::get<std::string>(it_process_executable_name->second);
    }
    resource.attributes_[OTEL_GET_RESOURCE_ATTR(AttrServiceName)] = default_service_name;
  }
  return resource;
}

Resource &Resource::GetEmpty()
{
  static Resource empty_resource;
  return empty_resource;
}

Resource &Resource::GetDefault()
{
  static Resource default_resource(
      {{OTEL_GET_RESOURCE_ATTR(AttrTelemetrySdkLanguage), "cpp"},
       {OTEL_GET_RESOURCE_ATTR(AttrTelemetrySdkName), "opentelemetry"},
       {OTEL_GET_RESOURCE_ATTR(AttrTelemetrySdkVersion), OPENTELEMETRY_SDK_VERSION}},
      std::string{});
  return default_resource;
}

const ResourceAttributes &Resource::GetAttributes() const noexcept
{
  return attributes_;
}

const std::string &Resource::GetSchemaURL() const noexcept
{
  return schema_url_;
}

}  // namespace resource
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
