#pragma once

#include "opentelemetry/plugin/factory.h"

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
}  // namespace mock
}  // namespace sdk
}  // namespace opentelemetry
