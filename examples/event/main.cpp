//
// Example that illustrates the following concepts:
// - how to use ILogger-style event API
// - how to attach event::Properties object to span
// - how to implement a custom Tracer
//

#include <opentelemetry/event/Properties.hpp>

#include <opentelemetry/trace/key_value_iterable_view.h>
#include <opentelemetry/trace/span.h>
#include <opentelemetry/trace/span_id.h>
#include <opentelemetry/trace/trace_id.h>
#include <opentelemetry/trace/tracer_provider.h>

#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include <map>
#include <string_view>

#include "ConsoleTracer.hpp"

using namespace OPENTELEMETRY_NAMESPACE;

using EventProperties = event::Properties;
using UUID_t          = event::UUID;
using time_ticks      = event::time_ticks;
using Property        = event::Property;
using Attribute       = event::Attribute;

namespace trace = opentelemetry::trace;

/**
 * Print event in JSON format to console
 */
void LogEvent(EventProperties &event)
{
  printf("%s = \n", event.GetName().c_str());
  size_t size = event.GetProperties().size();
  size_t i    = 0;
  printf("{\n");
  for (auto &k : event.GetProperties())
  {
    auto &v = k.second;
    switch (k.second.type)
    {
      case Property::TYPE_STRING:
        printf("  \"%s\": \"%s\"", k.first.c_str(), k.second.as_string);
        break;
      case Property::TYPE_UUID:
        printf("  \"%s\": \"%s\"", k.first.c_str(), k.second.to_string().c_str());
        break;
      default:
        printf("  \"%s\": %s", k.first.c_str(), k.second.to_string().c_str());
        break;
    }
    i++;
    printf((i != size) ? ",\n" : "\n");
  }
  printf("}\n");
};

/**
 * Logging API similar to Microsoft 1DS C++ SDK
 */
void test_events()
{
  // Using initializer list to express a variant map
  EventProperties myEvent(
      // Define named event
      "MyEvent",
      // Event properties
      {
          // Various typed key-values
          {"strKey1", "hello1"},
          {"strKey2", "hello2"},
          {"int64Key", (int64_t)1L},
          {"dblKey", 3.14},
          {"boolKey", (bool)false},
          {"guidKey0", UUID_t("00000000-0000-0000-0000-000000000000")},
          {"guidKey1", UUID_t("00010203-0405-0607-0809-0A0B0C0D0E0F")},
          {"guidKey2", UUID_t("00010203-0405-0607-0809-0A0B0C0D0E0F")},
          {"timeKey1", time_ticks((uint64_t)0)},  // time in .NET ticks
      });
  LogEvent(myEvent);

  // Using setters/getters
  EventProperties myEvent2("MyEvent2");
  myEvent2.SetProperty("strKey1", "helloAgain");
  LogEvent(myEvent2);

  // Using attributes
  EventProperties myEvent3(
      "UserSubscribedEvent",
      {{"email", Property("maxgolov@fabrikam.com", Attribute::ATTRIB_EUII_Smtp)}});

  LogEvent(myEvent3);
}

/**
 * Transform from EventProperties to collection of variant (AttributeValue)
 */
void test_add_eventproperties_to_span(trace::Span& span)
{
  EventProperties myEvent( "MyProduct.MyEvent4", {{"key1", "value1"}});
  auto name = myEvent.GetName();
  span.AddEvent(nostd::string_view(name.c_str(), name.length()), myEvent);
}

/**
 * OpenTelemetry Tracer and Span API
 */
void test_spans()
{
  console::TracerProvider con;
  opentelemetry::core::SystemTimestamp now(std::chrono::system_clock::now());

  auto tracer = con.GetTracer("default", "1.0");
  auto span   = tracer->StartSpan("MySpan");

  using M = std::map<std::string_view, std::string_view>;

  // add m1 to span 1
  M m1 = {{"key1", "one"}, {"key2", "two"}};
  span->AddEvent("MyProduct.MyEvent1", m1);

  // add m2 to span 2
  M m2     = {{"key1", "one"}, {"key2", "two"}};
  span->AddEvent("MyProduct.MyEvent2", m2);

#if 0
  // add map to span using initializer_list
  span->AddEvent("MyProduct.MyEvent3", {{"key1", "one"}, {"key2", "two"}});

  // add EventProperties to span
  test_add_eventproperties_to_span(*span);
#endif

  span->End();

  // end tracing session
  tracer->Close();
}

int main(int argc, char *argv[])
{
  printf("*** \n");
  printf("*** test_events ...\n");
  printf("*** \n");
  test_events();

  printf("*** \n");
  printf("*** test_spans  ...\n");
  printf("*** \n");
  test_spans();

  return 0;
}
