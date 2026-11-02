// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/configuration/sdk_builder.h"

#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/common/kv_properties.h"
#include "opentelemetry/context/propagation/composite_propagator.h"
#include "opentelemetry/context/propagation/text_map_propagator.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/configuration/attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/attribute_value_configuration_visitor.h"
#include "opentelemetry/sdk/configuration/attributes_configuration.h"
#include "opentelemetry/sdk/configuration/boolean_array_attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/boolean_attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/configuration.h"
#include "opentelemetry/sdk/configuration/configured_sdk.h"
#include "opentelemetry/sdk/configuration/container_resource_detector_builder.h"
#include "opentelemetry/sdk/configuration/container_resource_detector_configuration.h"
#include "opentelemetry/sdk/configuration/double_array_attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/double_attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/extension_resource_detector_builder.h"
#include "opentelemetry/sdk/configuration/extension_resource_detector_configuration.h"
#include "opentelemetry/sdk/configuration/host_resource_detector_builder.h"
#include "opentelemetry/sdk/configuration/host_resource_detector_configuration.h"
#include "opentelemetry/sdk/configuration/include_exclude_configuration.h"
#include "opentelemetry/sdk/configuration/integer_array_attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/integer_attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/logger_provider_builder.h"
#include "opentelemetry/sdk/configuration/logger_provider_builder_context.h"
#include "opentelemetry/sdk/configuration/meter_provider_builder.h"
#include "opentelemetry/sdk/configuration/meter_provider_builder_context.h"
#include "opentelemetry/sdk/configuration/process_resource_detector_builder.h"
#include "opentelemetry/sdk/configuration/process_resource_detector_configuration.h"
#include "opentelemetry/sdk/configuration/propagator_configuration.h"
#include "opentelemetry/sdk/configuration/registry.h"
#include "opentelemetry/sdk/configuration/resource_configuration.h"
#include "opentelemetry/sdk/configuration/resource_detection_configuration.h"
#include "opentelemetry/sdk/configuration/resource_detector_configuration.h"
#include "opentelemetry/sdk/configuration/resource_detector_configuration_visitor.h"
#include "opentelemetry/sdk/configuration/service_resource_detector_builder.h"
#include "opentelemetry/sdk/configuration/service_resource_detector_configuration.h"
#include "opentelemetry/sdk/configuration/severity_number.h"
#include "opentelemetry/sdk/configuration/string_array_attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/string_array_configuration.h"
#include "opentelemetry/sdk/configuration/string_attribute_value_configuration.h"
#include "opentelemetry/sdk/configuration/text_map_propagator_builder.h"
#include "opentelemetry/sdk/configuration/tracer_provider_builder.h"
#include "opentelemetry/sdk/configuration/tracer_provider_builder_context.h"
#include "opentelemetry/sdk/configuration/unsupported_exception.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/sdk/resource/resource_detector.h"
#include "opentelemetry/version.h"
#include "src/common/wildcard_match.h"
#include "src/resource/detail/percent_decode.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

namespace
{

class ResourceAttributeValueSetter
    : public opentelemetry::sdk::configuration::AttributeValueConfigurationVisitor
{
public:
  ResourceAttributeValueSetter(
      opentelemetry::sdk::resource::ResourceAttributes &resource_attributes,
      const std::string &name)
      : resource_attributes_(resource_attributes), name_(name)
  {}
  ResourceAttributeValueSetter(ResourceAttributeValueSetter &&)                      = delete;
  ResourceAttributeValueSetter(const ResourceAttributeValueSetter &)                 = delete;
  ResourceAttributeValueSetter &operator=(ResourceAttributeValueSetter &&)           = delete;
  ResourceAttributeValueSetter &operator=(const ResourceAttributeValueSetter &other) = delete;
  ~ResourceAttributeValueSetter() override                                           = default;

  void VisitString(
      const opentelemetry::sdk::configuration::StringAttributeValueConfiguration *model) override
  {
    opentelemetry::common::AttributeValue attr_value(model->value);
    resource_attributes_.SetAttribute(name_, attr_value);
  }

  void VisitInteger(
      const opentelemetry::sdk::configuration::IntegerAttributeValueConfiguration *model) override
  {
    /* Provide exact type to opentelemetry::common::AttributeValue variant. */
    int64_t value = model->value;
    opentelemetry::common::AttributeValue attr_value(value);
    resource_attributes_.SetAttribute(name_, attr_value);
  }

  void VisitDouble(
      const opentelemetry::sdk::configuration::DoubleAttributeValueConfiguration *model) override
  {
    opentelemetry::common::AttributeValue attr_value(model->value);
    resource_attributes_.SetAttribute(name_, attr_value);
  }

  void VisitBoolean(
      const opentelemetry::sdk::configuration::BooleanAttributeValueConfiguration *model) override
  {
    opentelemetry::common::AttributeValue attr_value(model->value);
    resource_attributes_.SetAttribute(name_, attr_value);
  }

  void VisitStringArray(
      const opentelemetry::sdk::configuration::StringArrayAttributeValueConfiguration *model)
      override
  {
    size_t length = model->value.size();
    std::vector<nostd::string_view> string_view_array(length);

    // We have: std::vector<std::string>
    // We need: nostd::span<const nostd::string_view>

    for (size_t i = 0; i < length; i++)
    {
      string_view_array[i] = model->value[i];
    }

    nostd::span<const nostd::string_view> span(string_view_array.data(), string_view_array.size());

    opentelemetry::common::AttributeValue attr_value(span);
    resource_attributes_.SetAttribute(name_, attr_value);
  }

  void VisitIntegerArray(
      const opentelemetry::sdk::configuration::IntegerArrayAttributeValueConfiguration *model)
      override
  {
    size_t length = model->value.size();
    std::vector<int64_t> int_array(length);

    // We have: std::vector<size_t>
    // We need: nostd::span<const int64_t>

    for (size_t i = 0; i < length; i++)
    {
      int_array[i] = static_cast<int64_t>(model->value[i]);
    }

    nostd::span<const int64_t> span(int_array.data(), int_array.size());

    opentelemetry::common::AttributeValue attr_value(span);
    resource_attributes_.SetAttribute(name_, attr_value);
  }

  void VisitDoubleArray(
      const opentelemetry::sdk::configuration::DoubleArrayAttributeValueConfiguration *model)
      override
  {
    // We have: std::vector<double>
    // We need: nostd::span<const double>
    // so no data conversion needed

    nostd::span<const double> span(model->value.data(), model->value.size());

    opentelemetry::common::AttributeValue attr_value(span);
    resource_attributes_.SetAttribute(name_, attr_value);
  }

  void VisitBooleanArray(
      const opentelemetry::sdk::configuration::BooleanArrayAttributeValueConfiguration *model)
      override
  {
    size_t length = model->value.size();

    // Can not use std::vector<bool>,
    // it has no data() to convert it to a span
    std::unique_ptr<bool[]> bool_array(new bool[length]);

    // We have: std::vector<bool>
    // We need: nostd::span<const bool>

    for (size_t i = 0; i < length; i++)
    {
      bool_array[i] = model->value[i];
    }

    nostd::span<const bool> span(&bool_array[0], length);

    opentelemetry::common::AttributeValue attr_value(span);
    resource_attributes_.SetAttribute(name_, attr_value);
  }

  opentelemetry::common::AttributeValue attribute_value;

private:
  opentelemetry::sdk::resource::ResourceAttributes &resource_attributes_;
  std::string name_;
};

class ResourceDetectorBuilder
    : public opentelemetry::sdk::configuration::ResourceDetectorConfigurationVisitor
{
public:
  ResourceDetectorBuilder(const SdkBuilder *b) : sdk_builder_(b) {}
  ResourceDetectorBuilder(ResourceDetectorBuilder &&)                      = delete;
  ResourceDetectorBuilder(const ResourceDetectorBuilder &)                 = delete;
  ResourceDetectorBuilder &operator=(ResourceDetectorBuilder &&)           = delete;
  ResourceDetectorBuilder &operator=(const ResourceDetectorBuilder &other) = delete;
  ~ResourceDetectorBuilder() override                                      = default;

  void VisitContainer(
      const opentelemetry::sdk::configuration::ContainerResourceDetectorConfiguration *model)
      override
  {
    detector = sdk_builder_->CreateContainerResourceDetector(model);
  }

  void VisitHost(
      const opentelemetry::sdk::configuration::HostResourceDetectorConfiguration *model) override
  {
    detector = sdk_builder_->CreateHostResourceDetector(model);
  }

  void VisitProcess(
      const opentelemetry::sdk::configuration::ProcessResourceDetectorConfiguration *model) override
  {
    detector = sdk_builder_->CreateProcessResourceDetector(model);
  }

  void VisitService(
      const opentelemetry::sdk::configuration::ServiceResourceDetectorConfiguration *model) override
  {
    detector = sdk_builder_->CreateServiceResourceDetector(model);
  }

  void VisitExtension(
      const opentelemetry::sdk::configuration::ExtensionResourceDetectorConfiguration *model)
      override
  {
    detector = sdk_builder_->CreateExtensionResourceDetector(model);
  }

  std::unique_ptr<opentelemetry::sdk::resource::ResourceDetector> detector;

private:
  const SdkBuilder *sdk_builder_;
};

bool ResourceAttributeKeyMatches(
    const opentelemetry::sdk::configuration::IncludeExcludeConfiguration *attributes,
    const std::string &key)
{
  using opentelemetry::sdk::common::WildcardMatch;

  bool included = true;

  if (attributes->included != nullptr && !attributes->included->string_array.empty())
  {
    included = false;
    for (const auto &pattern : attributes->included->string_array)
    {
      if (WildcardMatch(pattern, key))
      {
        included = true;
        break;
      }
    }
  }

  if (!included)
  {
    return false;
  }

  // excluded is applied after included, and wins.
  if (attributes->excluded != nullptr)
  {
    for (const auto &pattern : attributes->excluded->string_array)
    {
      if (WildcardMatch(pattern, key))
      {
        return false;
      }
    }
  }

  return true;
}
}  // namespace

SdkBuilder::SdkBuilder(std::shared_ptr<Registry> registry) : registry_(std::move(registry)) {}

std::unique_ptr<opentelemetry::context::propagation::TextMapPropagator>
SdkBuilder::CreateTextMapPropagator(const std::string &name) const
{
  std::unique_ptr<opentelemetry::context::propagation::TextMapPropagator> sdk;

  const TextMapPropagatorBuilder *builder = registry_->GetTextMapPropagatorBuilder(name);

  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("[SDK Builder] CreateTextMapPropagator() using registered builder "
                            << name);
    sdk = builder->Build();
    return sdk;
  }

  std::string die("CreateTextMapPropagator() no builder for ");
  die.append(name);
  throw UnsupportedException(die);
}

static bool IsDuplicate(const std::vector<std::string> &propagator_seen, const std::string &name)
{
  bool duplicate = false;
  for (const auto &seen : propagator_seen)
  {
    if (name == seen)
    {
      duplicate = true;
    }
  }

  return duplicate;
}

std::unique_ptr<opentelemetry::context::propagation::TextMapPropagator>
SdkBuilder::CreatePropagator(
    const std::unique_ptr<opentelemetry::sdk::configuration::PropagatorConfiguration> &model) const
{
  std::unique_ptr<opentelemetry::context::propagation::CompositePropagator> sdk;
  std::vector<std::unique_ptr<opentelemetry::context::propagation::TextMapPropagator>> propagators;
  std::unique_ptr<opentelemetry::context::propagation::TextMapPropagator> propagator;
  std::vector<std::string> propagator_seen;
  bool duplicate = false;

  /*
   * Note that the spec only requires to check duplicates between
   * composite and composite_list.
   * Here we check for duplicates globally, for ease of use.
   */

  for (const auto &name : model->composite)
  {
    duplicate = IsDuplicate(propagator_seen, name);

    if (!duplicate)
    {
      propagator = CreateTextMapPropagator(name);
      propagators.push_back(std::move(propagator));
      propagator_seen.push_back(name);
    }
  }

  if (model->composite_list.size() > 0)
  {
    std::string str_list = model->composite_list;
    size_t start_pos     = 0;
    size_t end_pos       = 0;
    char separator       = ',';
    std::string name;

    while ((end_pos = str_list.find(separator, start_pos)) != std::string::npos)
    {
      name = str_list.substr(start_pos, end_pos - start_pos);

      duplicate = IsDuplicate(propagator_seen, name);

      if (!duplicate)
      {
        propagator = CreateTextMapPropagator(name);
        propagators.push_back(std::move(propagator));
        propagator_seen.push_back(name);
      }
      start_pos = end_pos + 1;
    }

    name = str_list.substr(start_pos);

    duplicate = IsDuplicate(propagator_seen, name);

    if (!duplicate)
    {
      propagator = CreateTextMapPropagator(name);
      propagators.push_back(std::move(propagator));
    }
  }

  if (propagators.size() > 0)
  {
    sdk = std::make_unique<opentelemetry::context::propagation::CompositePropagator>(
        std::move(propagators));
  }

  return sdk;
}

std::unique_ptr<opentelemetry::sdk::resource::ResourceDetector>
SdkBuilder::CreateContainerResourceDetector(
    const opentelemetry::sdk::configuration::ContainerResourceDetectorConfiguration *model) const
{
  const ContainerResourceDetectorBuilder *builder =
      registry_->GetContainerResourceDetectorBuilder();

  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG(
        "[SDK Builder] CreateContainerResourceDetector() using registered builder");
    return builder->Build(model);
  }

  static const std::string die("No builder for ContainerResourceDetector");
  throw UnsupportedException(die);
}

std::unique_ptr<opentelemetry::sdk::resource::ResourceDetector>
SdkBuilder::CreateHostResourceDetector(
    const opentelemetry::sdk::configuration::HostResourceDetectorConfiguration *model) const
{
  const HostResourceDetectorBuilder *builder = registry_->GetHostResourceDetectorBuilder();

  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG("[SDK Builder] CreateHostResourceDetector() using registered builder");
    return builder->Build(model);
  }

  static const std::string die("No builder for HostResourceDetector");
  throw UnsupportedException(die);
}

std::unique_ptr<opentelemetry::sdk::resource::ResourceDetector>
SdkBuilder::CreateProcessResourceDetector(
    const opentelemetry::sdk::configuration::ProcessResourceDetectorConfiguration *model) const
{
  const ProcessResourceDetectorBuilder *builder = registry_->GetProcessResourceDetectorBuilder();

  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG(
        "[SDK Builder] CreateProcessResourceDetector() using registered builder");
    return builder->Build(model);
  }

  static const std::string die("No builder for ProcessResourceDetector");
  throw UnsupportedException(die);
}

std::unique_ptr<opentelemetry::sdk::resource::ResourceDetector>
SdkBuilder::CreateServiceResourceDetector(
    const opentelemetry::sdk::configuration::ServiceResourceDetectorConfiguration *model) const
{
  const ServiceResourceDetectorBuilder *builder = registry_->GetServiceResourceDetectorBuilder();

  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG(
        "[SDK Builder] CreateServiceResourceDetector() using registered builder");
    return builder->Build(model);
  }

  static const std::string die("No builder for ServiceResourceDetector");
  throw UnsupportedException(die);
}

std::unique_ptr<opentelemetry::sdk::resource::ResourceDetector>
SdkBuilder::CreateExtensionResourceDetector(
    const opentelemetry::sdk::configuration::ExtensionResourceDetectorConfiguration *model) const
{
  std::string name = model->name;

  const ExtensionResourceDetectorBuilder *builder =
      registry_->GetExtensionResourceDetectorBuilder(name);

  if (builder != nullptr)
  {
    OTEL_INTERNAL_LOG_DEBUG(
        "[SDK Builder] CreateExtensionResourceDetector() using registered builder " << name);
    return builder->Build(model);
  }

  std::string die("CreateExtensionResourceDetector() no builder for ");
  die.append(name);
  throw UnsupportedException(die);
}

std::unique_ptr<opentelemetry::sdk::resource::ResourceDetector> SdkBuilder::CreateResourceDetector(
    const std::unique_ptr<opentelemetry::sdk::configuration::ResourceDetectorConfiguration> &model)
    const
{
  ResourceDetectorBuilder builder(this);
  model->Accept(&builder);
  return std::move(builder.detector);
}

opentelemetry::sdk::resource::Resource SdkBuilder::CreateDetectedResource(
    const std::unique_ptr<opentelemetry::sdk::configuration::ResourceDetectionConfiguration> &model)
    const
{
  opentelemetry::sdk::resource::Resource detected;

  for (const auto &detector_model : model->detectors)
  {
    auto detector = CreateResourceDetector(detector_model);
    detected      = detected.Merge(detector->Detect());
  }

  // The filter applies to detected attributes only.
  if (model->attributes != nullptr)
  {
    opentelemetry::sdk::resource::ResourceAttributes filtered_attributes;

    for (const auto &kv : detected.GetAttributes())
    {
      if (ResourceAttributeKeyMatches(model->attributes.get(), kv.first))
      {
        filtered_attributes[kv.first] = kv.second;
      }
    }

    detected = opentelemetry::sdk::resource::Resource(filtered_attributes, detected.GetSchemaURL());
  }

  return detected;
}

void SdkBuilder::SetResourceAttribute(
    opentelemetry::sdk::resource::ResourceAttributes &resource_attributes,
    const std::string &name,
    const opentelemetry::sdk::configuration::AttributeValueConfiguration *model) const
{
  ResourceAttributeValueSetter setter(resource_attributes, name);
  // Invokes resource_attributes.SetAttribute(name, <proper value from model>)
  model->Accept(&setter);
}

void SdkBuilder::SetResource(
    opentelemetry::sdk::resource::Resource &resource,
    const std::unique_ptr<opentelemetry::sdk::configuration::ResourceConfiguration> &opt_model)
    const
{
  // Lowest priority: the default resource, with telemetry.sdk.* attributes only.
  // Resource::Create() is not usable here: it also runs OTELResourceDetector,
  // which is not part of the configuration model.
  resource = opentelemetry::sdk::resource::Resource::GetDefault();

  if (opt_model)
  {
    // Detected attributes, filtered by detection.attributes, win over the default.
    if (opt_model->detection != nullptr)
    {
      resource = resource.Merge(CreateDetectedResource(opt_model->detection));
    }

    // attributes_list wins over detected attributes.
    if (opt_model->attributes_list.size() != 0)
    {
      opentelemetry::sdk::resource::ResourceAttributes list_attributes;

      opentelemetry::common::KeyValueStringTokenizer tokenizer{opt_model->attributes_list};

      opentelemetry::nostd::string_view attribute_key;
      opentelemetry::nostd::string_view attribute_value;
      bool attribute_valid = true;

      while (tokenizer.next(attribute_valid, attribute_key, attribute_value))
      {
        if (attribute_valid)
        {
          std::string decoded_value = opentelemetry::sdk::resource::detail::PercentDecode(
              std::string{attribute_value.data(), attribute_value.size()});

          opentelemetry::common::AttributeValue wrapped_attribute_value(decoded_value);
          list_attributes.SetAttribute(attribute_key, wrapped_attribute_value);
        }
        else
        {
          OTEL_INTERNAL_LOG_WARN("[SDK Builder] Found invalid key/value pair in attributes_list");
        }
      }

      resource = resource.Merge(opentelemetry::sdk::resource::Resource(list_attributes));
    }

    // Highest priority: attributes and schema_url from the model.
    opentelemetry::sdk::resource::ResourceAttributes sdk_attributes;

    if (opt_model->attributes)
    {
      for (const auto &kv : opt_model->attributes->kv_map)
      {
        // kv_map values may be nullptr for programmatically-set null entries.
        // The default behavior for resource.attributes is to ignore null values.
        if (kv.second != nullptr)
        {
          SetResourceAttribute(sdk_attributes, kv.first, kv.second.get());
        }
      }
    }

    resource = resource.Merge(
        opentelemetry::sdk::resource::Resource(sdk_attributes, opt_model->schema_url));
  }
}

void SdkBuilder::SetLogLevel(
    opentelemetry::sdk::common::internal_log::LogLevel &sdk_log_level,
    opentelemetry::sdk::configuration::SeverityNumber model_log_level) const
{
  sdk_log_level = opentelemetry::sdk::common::internal_log::LogLevel::Info;

  switch (model_log_level)
  {
    case SeverityNumber::trace:
    case SeverityNumber::trace2:
    case SeverityNumber::trace3:
    case SeverityNumber::trace4:
    case SeverityNumber::debug:
    case SeverityNumber::debug2:
    case SeverityNumber::debug3:
    case SeverityNumber::debug4:
      sdk_log_level = opentelemetry::sdk::common::internal_log::LogLevel::Debug;
      break;
    case SeverityNumber::info:
    case SeverityNumber::info2:
    case SeverityNumber::info3:
    case SeverityNumber::info4:
      sdk_log_level = opentelemetry::sdk::common::internal_log::LogLevel::Info;
      break;
    case SeverityNumber::warn:
    case SeverityNumber::warn2:
    case SeverityNumber::warn3:
    case SeverityNumber::warn4:
      sdk_log_level = opentelemetry::sdk::common::internal_log::LogLevel::Warning;
      break;
    case SeverityNumber::error:
    case SeverityNumber::error2:
    case SeverityNumber::error3:
    case SeverityNumber::error4:
    case SeverityNumber::fatal:
    case SeverityNumber::fatal2:
    case SeverityNumber::fatal3:
    case SeverityNumber::fatal4:
      sdk_log_level = opentelemetry::sdk::common::internal_log::LogLevel::Error;
      break;
  }
}

std::unique_ptr<ConfiguredSdk> SdkBuilder::CreateConfiguredSdk(
    const std::unique_ptr<opentelemetry::sdk::configuration::Configuration> &model) const
{
  auto sdk = std::make_unique<ConfiguredSdk>();

  SetLogLevel(sdk->log_level, model->log_level);

  if (!model->disabled)
  {
    SetResource(sdk->resource, model->resource);

    if (model->tracer_provider)
    {
      const TracerProviderBuilder *tp_builder = registry_->GetTracerProviderBuilder();
      if (tp_builder == nullptr)
      {
        static const std::string message("No builder for TracerProvider");
        throw UnsupportedException(message);
      }
      TracerProviderBuilderContext tp_context{registry_.get(), &sdk->resource,
                                              model->attribute_limits.get()};
      sdk->tracer_provider = tp_builder->Build(tp_context, model->tracer_provider.get());
      if (sdk->tracer_provider == nullptr)
      {
        static const std::string message("TracerProviderBuilder returned null provider");
        throw UnsupportedException(message);
      }
    }

    if (model->propagator)
    {
      sdk->propagator = CreatePropagator(model->propagator);
    }

    if (model->meter_provider)
    {
      const MeterProviderBuilder *mp_builder = registry_->GetMeterProviderBuilder();
      if (mp_builder == nullptr)
      {
        static const std::string message("No builder for MeterProvider");
        throw UnsupportedException(message);
      }
      MeterProviderBuilderContext mp_context{registry_.get(), &sdk->resource};
      sdk->meter_provider = mp_builder->Build(mp_context, model->meter_provider.get());
      if (sdk->meter_provider == nullptr)
      {
        static const std::string message("MeterProviderBuilder returned null provider");
        throw UnsupportedException(message);
      }
    }

    if (model->logger_provider)
    {
      const LoggerProviderBuilder *lp_builder = registry_->GetLoggerProviderBuilder();
      if (lp_builder == nullptr)
      {
        static const std::string message("No builder for LoggerProvider");
        throw UnsupportedException(message);
      }
      LoggerProviderBuilderContext lp_context{registry_.get(), &sdk->resource,
                                              model->attribute_limits.get()};
      sdk->logger_provider = lp_builder->Build(lp_context, model->logger_provider.get());
      if (sdk->logger_provider == nullptr)
      {
        static const std::string message("LoggerProviderBuilder returned null provider");
        throw UnsupportedException(message);
      }
    }

    if (model->distribution)
    {
      // FIXME-CONFIG: Implement distribution configuration support
      OTEL_INTERNAL_LOG_WARN("[SDK Builder] the distribution model is not yet supported, ignoring");
    }

    // FIXME-CONFIG: Implement instrumentation/development support
    // if(model->instrumentation)
    // {
    //   OTEL_INTERNAL_LOG_WARN("[SDK Builder] instrumentation is not yet supported, ignoring");
    // }
  }

  // Set the log level if the SDK has been created successfully.
  opentelemetry::sdk::common::internal_log::GlobalLogHandler::SetLogLevel(sdk->log_level);

  OTEL_INTERNAL_LOG_DEBUG("[SDK Builder] Configured SDK is "
                          << (model->disabled ? "disabled" : "enabled"));

  return sdk;
}

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
