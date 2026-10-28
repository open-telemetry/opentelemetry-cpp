// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/resource_detectors/service_detector.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/resource_detectors/detail/service_detector_utils.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/sdk/resource/resource_detector.h"
#include "opentelemetry/semconv/schema_url.h"
#include "opentelemetry/semconv/service_attributes.h"
#include "opentelemetry/version.h"

#include <string>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace resource_detector
{

opentelemetry::sdk::resource::Resource ServiceResourceDetector::Detect() noexcept
{
  opentelemetry::sdk::resource::ResourceAttributes attributes;
  attributes[semconv::service::kServiceName]       = detail::GetServiceName();
  attributes[semconv::service::kServiceInstanceId] = detail::GenerateServiceInstanceId();
  return ResourceDetector::Create(attributes, semconv::kSchemaUrl);
}

}  // namespace resource_detector
OPENTELEMETRY_END_NAMESPACE
