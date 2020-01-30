#pragma once

#include "opentelemetry/plugin/factory.h"
#include "opentelemetry/plugin/loader_info.h"

namespace opentelemetry
{
namespace sdk
{
namespace mock
{
class FactoryImpl final : public plugin::Factory::FactoryImpl {
  // plugin::Factory::FactoryImpl
  nostd::unique_ptr<plugin::TracerHandle> MakeTracerHandle(
      nostd::string_view tracer_config,
      nostd::unique_ptr<char[]> &error_message) const noexcept override;
};

nostd::unique_ptr<plugin::Factory::FactoryImpl> MakeFactoryImpl(
    const plugin::LoaderInfo &loader_info,
    nostd::unique_ptr<char[]> &error_message) noexcept;
}  // namespace mock
}  // namespace sdk
}  // namespace opentelemetry
