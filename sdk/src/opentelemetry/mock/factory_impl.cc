#include "sdk/src/opentelemetry/mock/factory_impl.h"

namespace opentelemetry
{
namespace sdk
{
namespace mock
{
//--------------------------------------------------------------------------------------------------
// MakeTracerHandle
//--------------------------------------------------------------------------------------------------
nostd::unique_ptr<plugin::TracerHandle> FactoryImpl::MakeTracerHandle(
    nostd::string_view tracer_config,
    nostd::unique_ptr<char[]> &error_message) const noexcept
{
  (void)tracer_config;
  (void)error_message;
  return nullptr;
}
}  // namespace mock
}  // namespace sdk
}  // namespace opentelemetry
