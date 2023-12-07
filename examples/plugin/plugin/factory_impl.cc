// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/plugin/factory.h"
#include "opentelemetry/plugin/hook.h"
#include "tracer.h"
#include "opentelemetry/plugin/dynamic_load.h"

#include <iostream>

using namespace opentelemetry;

class TracerHandle final : public plugin::TracerHandle
{
public:
  explicit TracerHandle(std::shared_ptr<Tracer> &&tracer) noexcept : tracer_{std::move(tracer)} {
    std::cout << "TracerHandle...\n";

  }

  // opentelemetry::plugin::TracerHandle
  Tracer &tracer() const noexcept override { 
    std::cout << "Tracer & tracer()\n";
    return *tracer_; 
    }

private:
  std::shared_ptr<Tracer> tracer_;
};

class FactoryImpl final : public plugin::Factory::FactoryImpl
{
public:
  // opentelemetry::plugin::Factory::FactoryImpl
  nostd::unique_ptr<plugin::TracerHandle> MakeTracerHandle(
      nostd::string_view /* tracer_config */,
      nostd::unique_ptr<char[]> & /* error_message */) const noexcept override
  {
    std::cout << "MakeTracerHandle...\n";
    std::shared_ptr<Tracer> tracer{new (std::nothrow) Tracer{""}};
    if (tracer == nullptr)
    {
      return nullptr;
    }
    return nostd::unique_ptr<TracerHandle>{new (std::nothrow) TracerHandle{std::move(tracer)}};
  }
};

static nostd::unique_ptr<plugin::Factory::FactoryImpl> MakeFactoryImpl(
    const plugin::LoaderInfo &loader_info,
    nostd::unique_ptr<char[]> &error_message) noexcept
{
  (void)error_message;
  printf("MakeFactoryImpl: version=%*s abi_version=%*s\n", 
    int(loader_info.opentelemetry_version.size()), 
    loader_info.opentelemetry_version.data(),
    int(loader_info.opentelemetry_abi_version.size()), 
    loader_info.opentelemetry_abi_version.data()
  );
  return nostd::unique_ptr<plugin::Factory::FactoryImpl>{new (std::nothrow) FactoryImpl{}};
}

OPENTELEMETRY_DEFINE_PLUGIN_HOOK(MakeFactoryImpl)

#ifdef _WIN32

static const char kProcessAttach[] = "PLUGIN: DLL_PROCESS_ATTACH\n";
static const char kThreadAttach[] = "PLUGIN: DLL_THREAD_ATTACH\n";
static const char kProcessDetach[] = "PLUGIN: DLL_PROCESS_DETACH\n";
static const char kThreadDetach[] = "PLUGIN: DLL_THREAD_DETACH\n";

BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,  // handle to DLL module
    DWORD fdwReason,     // reason for calling function
    LPVOID lpvReserved )  // reserved
{
    // Perform actions based on the reason for calling.
    switch( fdwReason ) 
    { 
        case DLL_PROCESS_ATTACH:
            WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), kProcessAttach, sizeof(kProcessAttach), nullptr, nullptr );
            FlushFileBuffers(GetStdHandle(STD_OUTPUT_HANDLE));
            break;

        case DLL_THREAD_ATTACH:
            WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), kProcessDetach, sizeof(kProcessDetach), nullptr, nullptr );
            FlushFileBuffers(GetStdHandle(STD_OUTPUT_HANDLE));
            break;

        case DLL_THREAD_DETACH:
            WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), kThreadDetach, sizeof(kThreadDetach), nullptr, nullptr );
            FlushFileBuffers(GetStdHandle(STD_OUTPUT_HANDLE));
            break;

        case DLL_PROCESS_DETACH:
            WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), kProcessDetach, sizeof(kProcessDetach), nullptr, nullptr );
            FlushFileBuffers(GetStdHandle(STD_OUTPUT_HANDLE));
            break;
    }
    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}
#endif