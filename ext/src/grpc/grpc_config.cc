#include "opentelemetry/ext/grpc/grpc_config.h"

// including from grpc's innards
#include "src/core/config/config_vars.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace ext
{
namespace grpc
{

std::string OverridesToString()
{
    return grpc_core::ConfigVars::Get().ToString();
}
    
void ResetOverrides()
{
    grpc_core::ConfigVars::Reset();
}

void SetOverrides(const Overrides& o)
{
    grpc_core::ConfigVars::Overrides go{};

    #define OPENTELEMETRY_EXT_GRPC_OVERRIDES_X_COPY_TO(_,name,__) go.name = o.name;
    OPENTELEMETRY_EXT_GRPC_OVERRIDES_X_ENUM(OPENTELEMETRY_EXT_GRPC_OVERRIDES_X_COPY_TO)

    grpc_core::ConfigVars::SetOverrides(go);
}

OPENTELEMETRY_EXPORT Overrides GetOverrides()
{
    const auto& configVars{ grpc_core::ConfigVars::Get() };
    Overrides o{};

    #define OPENTELEMETRY_EXT_GRPC_OVERRIDES_X_COPY_FROM(_,name,prop) o.name = configVars.prop();
    OPENTELEMETRY_EXT_GRPC_OVERRIDES_X_ENUM(OPENTELEMETRY_EXT_GRPC_OVERRIDES_X_COPY_FROM)

    return o;
}

}  // namespace grpc
}  // namespace ext
OPENTELEMETRY_END_NAMESPACE