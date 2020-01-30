#pragma once

#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/version.h"

namespace opentelemetry
{
namespace plugin
{
struct LoaderInfo
{
  nostd::string_view opentelemetry_version     = OPENTELEMETRY_VERSION;
  nostd::string_view opentelemetry_abi_version = OPENTELEMETRY_ABI_VERSION;
};
}  // namespace plugin
}  // namespace opentelemetry
