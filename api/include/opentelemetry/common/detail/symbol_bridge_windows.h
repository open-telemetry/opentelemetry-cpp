// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/version.h"

#ifndef OPENTELEMETRY_SYMBOL_BRIDGE_NOINLINE
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  include <Windows.h>
#endif

OPENTELEMETRY_BEGIN_NAMESPACE
namespace common
{
namespace detail
{

struct LoadedBridgeSymbol
{
  void* address;
  bool try_again;
};

LoadedBridgeSymbol LoadSymbolBridgeSymbol(const char* symbol_name) noexcept;

template<typename Signature, typename Disambiguate = void>
Signature LoadSymbolBridgeSymbol(const char* symbol_name) noexcept
{
  static LoadedBridgeSymbol symbol{nullptr, true};
  if (!symbol.address && symbol.try_again)
  {
    symbol = LoadSymbolBridgeSymbol(symbol_name);
  }
  return reinterpret_cast<Signature>(symbol.address);
}

#ifndef OPENTELEMETRY_SYMBOL_BRIDGE_NOINLINE

#ifndef OPENTELEMETRY_SYMBOL_BRIDGE_IMPL
inline
#endif
LoadedBridgeSymbol LoadSymbolBridgeSymbol(const char* symbol_name) noexcept
{
  struct ModuleLifetime
  {
    ::HMODULE module{nullptr};
    bool load_failed{false};

    ~ModuleLifetime()
    {
      if (module)
      {
        ::FreeLibrary(module);
      }
    }
  };

  static ::ATOM bridge_present_atom{0};
  if (bridge_present_atom == 0)
  {
    bridge_present_atom = ::FindAtomW(L"otelcpp-abi-bridge");
    if (bridge_present_atom == 0)
    {
      return {nullptr, true};
    }
  }

  static ModuleLifetime bridge_module{};
  if (!bridge_module.module)
  {
    if (bridge_module.load_failed)
    {
      return {nullptr, false};
    }

    ::GetModuleHandleExW(0x0, L"opentelemetry_api_symbol_bridge", &bridge_module.module);
    bridge_module.load_failed = !bridge_module.module;

    if (bridge_module.load_failed)
    {
      return {nullptr, false};
    }
  }

  const auto address = ::GetProcAddress(bridge_module.module, symbol_name);
  return {reinterpret_cast<void*>(address), false};
}
#endif

} // namespace detail
} // namespace common
OPENTELEMETRY_END_NAMESPACE
