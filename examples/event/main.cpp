#include <opentelemetry/event/Properties.hpp>

#include <cstdint>
#include <cstdio>
#include <cstdlib>

using namespace OPENTELEMETRY_NAMESPACE;

using EventProperties = event::Properties;
using UUID            = event::UUID;
using time_ticks      = event::time_ticks;
using Property        = event::Property;
using Attribute       = event::Attribute;

void LogEvent(EventProperties &event){

};

int main(int argc, char *argv[])
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
          {"guidKey0", UUID("00000000-0000-0000-0000-000000000000")},
          {"guidKey1", UUID("00010203-0405-0607-0809-0A0B0C0D0E0F")},
          {"guidKey2", UUID("00010203-0405-0607-0809-0A0B0C0D0E0F")},
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
  LogEvent(myEvent);

  printf("Hello!");
  return 0;
}
