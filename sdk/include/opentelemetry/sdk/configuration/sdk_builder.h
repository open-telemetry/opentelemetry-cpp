// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>
#include <memory>

#include "opentelemetry/sdk/configuration/attribute_limits_configuration.h"
#include "opentelemetry/sdk/configuration/batch_log_record_processor_configuration.h"
#include "opentelemetry/sdk/configuration/batch_span_processor_configuration.h"
#include "opentelemetry/sdk/configuration/boolean_array_attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/boolean_attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/configuration.h"
#include "opentelemetry/sdk/configuration/configured_sdk.h"
#include "opentelemetry/sdk/configuration/console_log_record_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/console_push_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/container_resource_detector_configuration.h"
#include "opentelemetry/sdk/configuration/double_array_attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/double_attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/extension_resource_detector_configuration.h"
#include "opentelemetry/sdk/configuration/host_resource_detector_configuration.h"
#include "opentelemetry/sdk/configuration/integer_array_attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/integer_attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/process_resource_detector_configuration.h"
#include "opentelemetry/sdk/configuration/registry.h"
#include "opentelemetry/sdk/configuration/resource_detection_configuration.h"
#include "opentelemetry/sdk/configuration/resource_detector_configuration.h"
#include "opentelemetry/sdk/configuration/service_resource_detector_configuration.h"
#include "opentelemetry/sdk/configuration/string_array_attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/string_attribute_value_configuration.h"
#include "opentelemetry/sdk/resource/resource_detector.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{

namespace configuration
{

class SdkBuilder
{
public:
  SdkBuilder(std::shared_ptr<Registry> registry) : registry_(std::move(registry)) {}
  SdkBuilder(SdkBuilder &&)                      = default;
  SdkBuilder(const SdkBuilder &)                 = default;
  SdkBuilder &operator=(SdkBuilder &&)           = default;
  SdkBuilder &operator=(const SdkBuilder &other) = default;
  ~SdkBuilder()                                  = default;

  std::unique_ptr<opentelemetry::context::propagation::TextMapPropagator> CreateTextMapPropagator(
      const std::string &name) const;

  std::unique_ptr<opentelemetry::context::propagation::TextMapPropagator> CreatePropagator(
      const std::unique_ptr<opentelemetry::sdk::configuration::PropagatorConfiguration> &model)
      const;

  std::unique_ptr<opentelemetry::sdk::resource::ResourceDetector> CreateContainerResourceDetector(
      const opentelemetry::sdk::configuration::ContainerResourceDetectorConfiguration *model) const;

  std::unique_ptr<opentelemetry::sdk::resource::ResourceDetector> CreateHostResourceDetector(
      const opentelemetry::sdk::configuration::HostResourceDetectorConfiguration *model) const;

  std::unique_ptr<opentelemetry::sdk::resource::ResourceDetector> CreateProcessResourceDetector(
      const opentelemetry::sdk::configuration::ProcessResourceDetectorConfiguration *model) const;

  std::unique_ptr<opentelemetry::sdk::resource::ResourceDetector> CreateServiceResourceDetector(
      const opentelemetry::sdk::configuration::ServiceResourceDetectorConfiguration *model) const;

  std::unique_ptr<opentelemetry::sdk::resource::ResourceDetector> CreateExtensionResourceDetector(
      const opentelemetry::sdk::configuration::ExtensionResourceDetectorConfiguration *model) const;

  std::unique_ptr<opentelemetry::sdk::resource::ResourceDetector> CreateResourceDetector(
      const std::unique_ptr<opentelemetry::sdk::configuration::ResourceDetectorConfiguration>
          &model) const;

  opentelemetry::sdk::resource::Resource CreateDetectedResource(
      const std::unique_ptr<opentelemetry::sdk::configuration::ResourceDetectionConfiguration>
          &model) const;

  void SetResourceAttribute(
      opentelemetry::sdk::resource::ResourceAttributes &resource_attributes,
      const std::string &name,
      const opentelemetry::sdk::configuration::AttributeValueConfiguration *model) const;

  void SetResource(opentelemetry::sdk::resource::Resource &resource,
                   const std::unique_ptr<opentelemetry::sdk::configuration::ResourceConfiguration>
                       &opt_model) const;

  void SetLogLevel(opentelemetry::sdk::common::internal_log::LogLevel &sdk_log_level,
                   opentelemetry::sdk::configuration::SeverityNumber model_log_level) const;

  std::unique_ptr<ConfiguredSdk> CreateConfiguredSdk(
      const std::unique_ptr<opentelemetry::sdk::configuration::Configuration> &model) const;

private:
  std::shared_ptr<Registry> registry_;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
