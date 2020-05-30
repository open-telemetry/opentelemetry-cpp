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

class EventSpan : public Span
{
  Tracer &parentTracer;

public:
  EventSpan(Tracer &parent) : parentTracer(parent) {}

  // Adds an event to the Span.
  virtual void AddEvent(nostd::string_view name) noexcept {}

  // Adds an event to the Span, with a custom timestamp.
  virtual void AddEvent(nostd::string_view name, core::SystemTimestamp timestamp) noexcept {}

  // Adds an event to the Span, with a custom timestamp, and attributes.
  virtual void AddEvent(nostd::string_view name,
                        core::SystemTimestamp timestamp,
                        const KeyValueIterable &attributes) noexcept
  {}

  // TODO: allow EventProperties to interop with KeyValueIterableView<T>
  virtual void AddEvent(nostd::string_view name, const EventProperties &attributes) noexcept {}

  // Sets the status of the span. The default status is OK. Only the value of the last call will be
  // recorded, and implementations are free to ignore previous calls.
  virtual void SetStatus(CanonicalCode code, nostd::string_view description) noexcept {

  };

  // Updates the name of the Span. If used, this will override the name provided
  // during creation.
  virtual void UpdateName(nostd::string_view name) noexcept {}

  // Mark the end of the Span. Only the timing of the first End call for a given Span will
  // be recorded, and implementations are free to ignore all further calls.
  virtual void End() noexcept {}

  virtual bool IsRecording() const noexcept {}

  virtual Tracer &tracer() const noexcept { return parentTracer; }
};

//
// Print event in JSON format to console
//
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

void test_spans()
{
  ConsoleTracerProvider con;
  opentelemetry::core::SystemTimestamp now(std::chrono::system_clock::now());

  auto tracer = con.GetTracer("default", "1.0");
  auto span   = tracer->StartSpan("MySpan");

  using M = std::map<std::string_view, std::string_view>;
  M m = {{"key1", "one"}, {"key2", "two"}};
  // trace::KeyValueIterableView<M> iterable{m};
  // iterable.size();
  span->AddEvent("MyEvent", m);

}

int main(int argc, char *argv[])
{
  test_events();
  return 0;
}
