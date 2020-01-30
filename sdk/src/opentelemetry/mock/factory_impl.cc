#include "sdk/src/opentelemetry/mock/factory_impl.h"

namespace opentelemetry
{
namespace sdk
{
namespace mock
{
nostd::unique_ptr<plugin::TracerHandle> FactoryImpl::MakeTracerHandle(
    nostd::string_view tracer_config,
    nostd::unique_ptr<char[]> &error_message) const noexcept
{
  (void)tracer_config;
  (void)error_message;
  return nullptr;
}

nostd::unique_ptr<plugin::Factory::FactoryImpl> MakeFactoryImpl(
    const plugin::LoaderInfo &loader_info,
    nostd::unique_ptr<char[]> &error_message) noexcept
{
  return nostd::unique_ptr<plugin::Factory::FactoryImpl>{new (std::nothrow) FactoryImpl{}};
}
}  // namespace mock
}  // namespace sdk
}  // namespace opentelemetry
