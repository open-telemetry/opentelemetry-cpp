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

#ifdef _WIN32

#  include <gtest/gtest.h>
#  include <map>
#  include <string>

#  include "opentelemetry/exporters/etw/etw_tracer_exporter.h"
#  include "opentelemetry/sdk/trace/simple_processor.h"

using namespace OPENTELEMETRY_NAMESPACE;

using Properties       = opentelemetry::exporter::etw::Properties;
using PropertyValue    = opentelemetry::exporter::etw::PropertyValue;
using PropertyValueMap = opentelemetry::exporter::etw::PropertyValueMap;

std::string getTemporaryValue()
{
  return std::string("Value from Temporary std::string");
}

/* clang-format off */
TEST(ETWTracer, TracerCheck)
{
  // SDK customer specifies their unique ETW ProviderName. Every component or library
  // is assumed to have its own instrumentation name. Traces are routed to dedicated
  // provider. Standard hash function maps from ProviderName to ProviderGUID.
  //
  // Prominent naming examples from `logman query providers` :
  //
  // [Docker]                                 {a3693192-9ed6-46d2-a981-f8226c8363bd}
  // ...
  // Intel-Autologger-iclsClient              {B8D7E9A0-65D5-40BE-AFEA-83593FC0164E}
  // Intel-Autologger-iclsProxy                {301B773F-50F3-4C8E-83F0-53BA9590A13E}
  // Intel-Autologger-PTTEKRecertification    {F33E9E07-8792-47E8-B3FA-2C92AB32C5B3}
  // ...
  // NodeJS-ETW-provider                      {77754E9B-264B-4D8D-B981-E4135C1ECB0C}
  // ...
  // OpenSSH                                  {C4B57D35-0636-4BC3-A262-370F249F9802}
  // ...
  // Windows Connect Now                      {C100BECE-D33A-4A4B-BF23-BBEF4663D017}
  // Windows Defender Firewall API            {28C9F48F-D244-45A8-842F-DC9FBC9B6E92}
  // Windows Defender Firewall API - GP       {0EFF663F-8B6E-4E6D-8182-087A8EAA29CB}
  // Windows Defender Firewall Driver         {D5E09122-D0B2-4235-ADC1-C89FAAAF1069}

  std::string providerName = "OpenTelemetry-ETW-TLD"; // supply unique instrumentation name here
  exporter::etw::TracerProvider tp;

  // TODO: this code should fallback to MsgPack if TLD is not available
  auto tracer = tp.GetTracer(providerName, "TLD");

  // Span attributes
  Properties attribs =
  {
    {"attrib1", 1},
    {"attrib2", 2}
  };

  auto topSpan = tracer->StartSpan("MySpanTop");
  std::this_thread::sleep_for (std::chrono::seconds(1));

  auto outerSpan = tracer->StartSpan("MySpanL2", attribs);

  // Create nested span. Note how we share the attributes here.
  // It is Okay to either reuse/share or have your own attributes.
  auto innerSpan = tracer->StartSpan("MySpanL3", attribs);

  // Add first event
  std::string eventName1 = "MyEvent1";
  Properties event1 =
  {
    {"uint32Key", (uint32_t)1234},
    {"uint64Key", (uint64_t)1234567890},
    {"strKey", "someValue"}
  };
  EXPECT_NO_THROW(outerSpan->AddEvent(eventName1, event1));
  std::this_thread::sleep_for (std::chrono::seconds(1));

  // Add second event
  std::string eventName2 = "MyEvent2";
  Properties event2 =
  {
    {"uint32Key", (uint32_t)9876},
    {"uint64Key", (uint64_t)987654321},
    {"strKey", "anotherValue"}
  };
  EXPECT_NO_THROW(outerSpan->AddEvent(eventName2, event2));
  std::this_thread::sleep_for (std::chrono::seconds(2));

  std::string eventName3= "MyEvent3";
    Properties event3 =
  {
    /* Extra metadata that allows event to flow to A.I. pipeline */
    {"metadata", "ai_event"},
    {"uint32Key", (uint32_t)9876},
    {"uint64Key", (uint64_t)987654321},
    // {"int32array", {{-1,0,1,2,3}} },
    {"tempString", getTemporaryValue() }
  };
  EXPECT_NO_THROW(innerSpan->AddEvent(eventName3, event3));
  std::this_thread::sleep_for (std::chrono::seconds(1));

  EXPECT_NO_THROW(innerSpan->End());    // end innerSpan

  EXPECT_NO_THROW(outerSpan->End());    // end outerSpan
  EXPECT_NO_THROW(topSpan->End());      // end topSpan

  EXPECT_NO_THROW(tracer->CloseWithMicroseconds(0));
}

// Lowest decoration level -> smaller ETW event size.
// Expected output in C# listener on the other side:
// no ActivityID GUID, no SpanId, no TraceId.
/*
{
  "Timestamp": "2021-03-19T21:04:38.411193-07:00",
  "ProviderName": "OpenTelemetry-ETW-Provider",
  "Id": 13,
  "Message": null,
  "ProcessId": 15120,
  "Level": "Always",
  "Keywords": "0x0000000000000000",
  "EventName": "C.min/Stop",
  "ActivityID": null,
  "RelatedActivityID": null,
  "Payload": {}
}
*/
TEST(ETWTracer, TracerCheckMinDecoration)
{
  std::string providerName = "OpenTelemetry-ETW-TLD";
  exporter::etw::TracerProvider tp
  ({
      {"enableTraceId", false},
      {"enableSpanId", false},
      {"enableActivityId", false},
      {"enableActivityTracking", true},
      {"enableRelatedActivityId", false},
      {"enableAutoParent", false}
  });
  auto tracer = tp.GetTracer(providerName, "TLD");
  auto aSpan = tracer->StartSpan("A.min");
  auto bSpan = tracer->StartSpan("B.min");
  auto cSpan = tracer->StartSpan("C.min");
  cSpan->End();
  bSpan->End();
  aSpan->End();
  tracer->CloseWithMicroseconds(0);
}

// Highest decoration level -> larger ETW event size
// Expected output in C# listener on the other side:
// ActivityID GUID (==SpanId), SpanId, TraceId.
/*
{
  "Timestamp": "2021-03-19T21:04:38.4120274-07:00",
  "ProviderName": "OpenTelemetry-ETW-Provider",
  "Id": 21,
  "Message": null,
  "ProcessId": 15120,
  "Level": "Always",
  "Keywords": "0x0000000000000000",
  "EventName": "C.max/Stop",
  "ActivityID": "d55a2c25-8033-40ab-0000-000000000000",
  "RelatedActivityID": null,
  "Payload": {
    "SpanId": "252c5ad53380ab40",
    "TraceId": "4dea2a63c188894ea5ab979e5cd7ec36"
  }
}
*/
TEST(ETWTracer, TracerCheckMaxDecoration)
{
  std::string providerName = "OpenTelemetry-ETW-TLD";
  exporter::etw::TracerProvider tp
  ({
      {"enableTraceId", true},
      {"enableSpanId", true},
      {"enableActivityId", true},
      {"enableRelatedActivityId", true},
      {"enableAutoParent", true}
  });
  auto tracer = tp.GetTracer(providerName, "TLD" );
  auto aSpan = tracer->StartSpan("A.max");
  auto bSpan = tracer->StartSpan("B.max");
  auto cSpan = tracer->StartSpan("C.max");
  cSpan->End();
  bSpan->End();
  aSpan->End();
  tracer->CloseWithMicroseconds(0);
}

TEST(ETWTracer, TracerCheckMsgPack)
{
  std::string providerName = "OpenTelemetry-ETW-MsgPack";
  exporter::etw::TracerProvider tp
  ({
      {"enableTraceId", true},
      {"enableSpanId", true},
      {"enableActivityId", true},
      {"enableRelatedActivityId", true},
      {"enableAutoParent", true}
  });
  auto tracer = tp.GetTracer(providerName, "MsgPack" );
  {
      auto aSpan = tracer->StartSpan("A.max");
      {
          auto bSpan = tracer->StartSpan("B.max");
          {
              auto cSpan = tracer->StartSpan("C.max");
              std::string eventName = "MyMsgPackEvent";
              Properties event =
              {
                  {"uint32Key", (uint32_t)1234},
                  {"uint64Key", (uint64_t)1234567890},
                  {"strKey", "someValue"}
              };
              cSpan->AddEvent(eventName, event);
              cSpan->End();
          }
          bSpan->End();
      }
      aSpan->End();
  }
  tracer->CloseWithMicroseconds(0);
}

/* clang-format on */

#endif
