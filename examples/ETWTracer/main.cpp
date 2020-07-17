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

// Option below requires C++17 + msgsl ( https://github.com/microsoft/GSL ) or C++20-compatible compiler (latest Visual Studio 2019)
// #define HAVE_CPP_STDLIB
#include "ETWTracer.hpp"

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

/// <summary>
/// OpenTelemetry C++ SDK API use example
/// </summary>
void test_OT_span_API()
{
  // TracerProvider is a Tracer factory.
  ETW::TracerProvider tp;

  //
  // MyProviderName => {b7aa4d18-240c-5f41-5852-817dbf477472}
  // Use 'StringToGUID' utility or utils::GetProviderGuid to obtain the GUID.
  //
  auto tracer = tp.GetTracer("MyProviderName");
  auto span   = tracer->StartSpan("MySpan");

  // Example 1: Basic illustration of ETW Event container.
  ETWEvent event = { {"uint32Key", (uint32_t)123456}, {"uint64Key", (uint64_t)123456}, {"strKey", "someValue"} };
  span->AddEvent("MyEvent1", event);

  // Example 2: Add map to span using initializer_list without intermediate ETWEvent container.
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
  // Convert Event to KeyValueIterableView + set event name as a separate parameter
  span->AddEvent(nostd::string_view(name.c_str(), name.length()), myEvent);

  // Conclude the span
  span->End();

  // end tracing session
  tracer->Close();
};

/// <summary>
/// Direct access to ETWProvider
/// </summary>
void test_ETWProvider_direct()
{
#ifdef HAVE_ETW_PROVIDER
  using ETWEvent = std::map<nostd::string_view, common::AttributeValue>;
  static ETWProvider etw;
  //
  // MyProviderName => {b7aa4d18-240c-5f41-5852-817dbf477472}
  // Use 'StringToGUID' utility or utils::GetProviderGuid to obtain the GUID.
  //
  const char *providerId = "MyProviderName";
  auto handle = etw.open(providerId);
  ETWEvent event =
  {
      {"name", "MyEvent3"},
      {"uint32Key", (uint32_t)123456},
      {"uint64Key", (uint64_t)123456},
      {"strKey", "someValue"}
  };
  etw.write(handle, event);
  etw.close(handle);
#endif
};

int main(int argc, char *argv[])
{
  // test_ETWProvider_direct();
  test_OT_span_API();
  return 0;
}
