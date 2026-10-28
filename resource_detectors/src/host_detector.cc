// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/resource_detectors/host_detector.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/resource_detectors/detail/host_detector_utils.h"
#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/sdk/resource/resource_detector.h"
#include "opentelemetry/semconv/incubating/host_attributes.h"
#include "opentelemetry/semconv/schema_url.h"
#include "opentelemetry/version.h"

#include <exception>
#include <ostream>
#include <string>
#include <utility>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace resource_detector
{

opentelemetry::sdk::resource::Resource HostResourceDetector::Detect() noexcept
{
  opentelemetry::sdk::resource::ResourceAttributes attributes;

  try
  {
    std::string host_name = opentelemetry::resource_detector::detail::GetHostName();
    if (!host_name.empty())
    {
      attributes[semconv::host::kHostName] = std::move(host_name);
    }
  }
  catch (const std::exception &ex)
  {
    OTEL_INTERNAL_LOG_ERROR("[Host Resource Detector] " << "Error extracting the host name: "
                                                        << ex.what());
  }

  std::string host_arch = opentelemetry::resource_detector::detail::GetHostArch();
  if (!host_arch.empty())
  {
    attributes[semconv::host::kHostArch] = std::move(host_arch);
  }

  try
  {
    std::string host_id = opentelemetry::resource_detector::detail::GetHostId();
    if (!host_id.empty())
    {
      attributes[semconv::host::kHostId] = std::move(host_id);
    }
  }
  catch (const std::exception &ex)
  {
    OTEL_INTERNAL_LOG_ERROR("[Host Resource Detector] " << "Error extracting the host id: "
                                                        << ex.what());
  }

  return ResourceDetector::Create(attributes, semconv::kSchemaUrl);
}

}  // namespace resource_detector
OPENTELEMETRY_END_NAMESPACE
