// Copyright 2020, OpenTelemetry Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Example that illustrates the following concepts:
// - how to use ILogger-style Event API
// - how to attach event::Properties object to span
// - how to implement a custom ETW Tracer
// - how to use ETW Tracer

// Option below requires C++17 + msgsl ( https://github.com/microsoft/GSL ) or C++20-compatible compiler (latest Visual Studio 2019) :
// #define HAVE_CPP_STDLIB
// Option below requires Abseil C++ library :
// #define HAVE_ABSEIL

#include "ETWTracer.hpp"
#include <string>

using namespace OPENTELEMETRY_NAMESPACE;

// Convenience alias for map of variant objects.
using ETWEvent        = std::map<nostd::string_view, common::AttributeValue>;

// DRAFT - unofficial / extended eventing API compatible with 1DS API surface
#include <opentelemetry/event/Properties.hpp>

using EventProperties = event::Properties;
using UUID_t          = event::UUID;
using time_ticks      = event::time_ticks;
using Property        = event::Property;
using Attribute       = event::Attribute;

void printProvider(std::string providerName)
{
  if (providerName.at(0) != '{')
  {
    auto guid = utils::GetProviderGuid(providerName.c_str());
    event::UUID uuid(guid);
    auto guidStr = uuid.to_string();
    printf("Provider Name: %s\n", providerName.c_str());
    printf("Provider GUID: %s\n", guidStr.c_str());
  }
  else
  {
    printf("Provider GUID: %s\n", providerName.c_str());
  }
};

    /// <summary>
/// OpenTelemetry C++ SDK API use example
/// </summary>
void test_OT_span_API(std::string providerName, std::string eventName)
{
  printf("Testing span API...\n");
  printProvider(providerName);
  printf("Event name:    %s\n", eventName.c_str());

  // TracerProvider is a Tracer factory.
  ETW::TracerProvider tp;

  // Use 'StringToGUID' utility or utils::GetProviderGuid to obtain the GUID.
  auto tracer = tp.GetTracer(providerName);
  printf("StartSpan: MySpan\n");
  auto span   = tracer->StartSpan("MySpan");

  // Example 1: Basic illustration of ETW Event container.
  ETWEvent event = { {"uint32Key", (uint32_t)123456}, {"uint64Key", (uint64_t)123456}, {"strKey", "someValue"} };
  printf("AddEvent: %s\n", eventName.c_str());
  span->AddEvent(eventName, event);

  // Example 2: Add map to span using initializer_list without intermediate ETWEvent container.
  printf("AddEvent: MyEvent2\n");
  span->AddEvent("MyEvent2", {{"key1", "one"}, {"key2", "two"}});

  // DRAFT - unofficial / extended eventing API compatible with 1DS API surface.
  // This API surface enhances the basic OpenTelemetry type system with additional
  // types such as GUID. It may also encapsulate a type to provide a transform from
  // UTF-16 string to either UTF-8 string or array of 16-bit unsigned integers.
  EventProperties myEvent(
      "MyEvent1DS",
      {/* C-string */ {"key1", "value1"},
       /* int32_t  */ {"intKey", 12345},
       /* bool     */ {"boolKey", static_cast<bool>(true)},
       /* GUID     */ {"guidKey1", UUID_t("00010203-0405-0607-0809-0A0B0C0D0E0F")}});
  auto name = myEvent.GetName();
  printf("AddEvent: %s\n", name.c_str());
  // Convert Event to KeyValueIterableView + set event name as a separate parameter
  span->AddEvent(nostd::string_view(name.c_str(), name.length()), myEvent);

  // Conclude the span
  printf("EndSpan\n");
  span->End();

  // end tracing session
  tracer->Close();
  printf("[ DONE ]\n");
};

/// <summary>
/// Direct access to ETWProvider
/// </summary>
void test_ETWProvider_direct(std::string providerName, std::string eventName)
{
  printf("Testing ETW logging API...\n");
  using ETWEvent = std::map<nostd::string_view, common::AttributeValue>;
  static ETWProvider etw;
  //
  // MyProviderName => {b7aa4d18-240c-5f41-5852-817dbf477472}
  // Use 'StringToGUID' utility or utils::GetProviderGuid to obtain the GUID.
  //
  printProvider(providerName);
  printf("Event name:    %s\n", eventName.c_str());
  auto handle            = etw.open(providerName.c_str());
  if (handle.providerHandle == ETWProvider::INVALID_HANDLE)
  {
    printf("Failed to register provider!\n");
    return;
  }
  printf("Provider Handle: 0x%08llx\n", handle.providerHandle);
  ETWEvent event =
  {
      {"name", eventName},
      {"uint32Key", (uint32_t)123456},
      {"uint64Key", (uint64_t)123456},
      {"strKey", "someValue"}
  };
  printf("etw.write...\n");
  etw.write(handle, event);
  printf("etw.close...\n");
  etw.close(handle);
  printf("[ DONE ]\n");
};

int main(int argc, char *argv[])
{
  std::string providerName = "MyProviderName";
  std::string eventName    = "MyEvent";
  if (argc>1)
  {
    providerName = argv[1];
    eventName    = argv[2];
  }

  test_OT_span_API(providerName, eventName);
  test_ETWProvider_direct(providerName, eventName);

  return 0;
}
