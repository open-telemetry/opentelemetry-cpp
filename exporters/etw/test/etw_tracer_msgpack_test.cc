// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <windows.h>

#include <evntprov.h>

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

namespace
{
int64_t current_time_ms = 1700000000000;

struct CapturedEvent
{
  UCHAR opcode;
  nlohmann::json message;
};

std::vector<CapturedEvent> events;

void WINAPI TestGetSystemTimeAsFileTime(LPFILETIME time)
{
  ULARGE_INTEGER value;
  value.QuadPart       = 116444736000000000ULL + static_cast<uint64_t>(current_time_ms) * 10000;
  time->dwLowDateTime  = value.LowPart;
  time->dwHighDateTime = value.HighPart;
}

ULONG CaptureEventWrite(REGHANDLE,
                        const EVENT_DESCRIPTOR *descriptor,
                        ULONG count,
                        const EVENT_DATA_DESCRIPTOR *data)
{
  EXPECT_EQ(descriptor->Id, 100);
  EXPECT_EQ(count, 1u);
  if (count != 1)
  {
    return ERROR_INVALID_PARAMETER;
  }
  const auto *bytes = reinterpret_cast<const uint8_t *>(static_cast<uintptr_t>(data[0].Ptr));
  events.push_back({descriptor->Opcode, nlohmann::json::from_msgpack(bytes, bytes + data[0].Size)});
  if (descriptor->Opcode == 2)
  {
    // Force the final Span write into a different millisecond without sleeping.
    ++current_time_ms;
  }
  return ERROR_SUCCESS;
}

ULONG CaptureEventWriteTransfer(REGHANDLE handle,
                                const EVENT_DESCRIPTOR *descriptor,
                                LPCGUID,
                                LPCGUID,
                                ULONG count,
                                const EVENT_DATA_DESCRIPTOR *data)
{
  return CaptureEventWrite(handle, descriptor, count, data);
}
}  // namespace

#define GetSystemTimeAsFileTime TestGetSystemTimeAsFileTime
#define EventWrite CaptureEventWrite
#define EventWriteTransfer CaptureEventWriteTransfer
#include "opentelemetry/exporters/etw/etw_tracer_exporter.h"
#undef EventWriteTransfer
#undef EventWrite
#undef GetSystemTimeAsFileTime

using namespace OPENTELEMETRY_NAMESPACE;

namespace
{
void CheckEndSpanTime(bool supplied_time)
{
  current_time_ms = 1700000000000;
  events.clear();
  exporter::etw::TracerProvider provider(
      {{"encoding", std::string("MsgPack")}, {"enableActivityTracking", true}});
  auto tracer = provider.GetTracer("MessagePackEndSpanTime");
  exporter::etw::Properties attributes{{"custom", "unchanged"}};
  if (supplied_time)
  {
    attributes[ETW_FIELD_TIME] = int64_t{42};
  }
  auto span      = tracer->StartSpan("operation", attributes);
  auto *etw_span = static_cast<exporter::etw::Span *>(span.get());
  ASSERT_EQ(etw_span->GetAttributes().count(ETW_FIELD_TIME), supplied_time ? 1u : 0u);
  events.clear();

  const auto expected = supplied_time ? int64_t{42} : current_time_ms;
  span->SetStatus(trace::StatusCode::kOk);
  span->End();

  ASSERT_EQ(events.size(), 2u);
  EXPECT_EQ(events[0].opcode, 2);
  EXPECT_EQ(events[1].opcode, 0);
  EXPECT_EQ(events[0].message[0], "operation");
  EXPECT_EQ(events[1].message[0], ETW_VALUE_SPAN);
  for (const auto &event : events)
  {
    const auto &payload = event.message[1][0][1];
    ASSERT_TRUE(payload.contains(ETW_FIELD_TIME));
    EXPECT_EQ(payload[ETW_FIELD_TIME].get<int64_t>(), expected);
    EXPECT_EQ(payload["custom"], "unchanged");
  }
  EXPECT_EQ(current_time_ms, 1700000000001);
  EXPECT_EQ(etw_span->GetAttributes().count(ETW_FIELD_TIME), supplied_time ? 1u : 0u);
  if (supplied_time)
  {
    EXPECT_EQ(nostd::get<int64_t>(etw_span->GetAttributes().at(ETW_FIELD_TIME)), 42);
  }
  EXPECT_EQ(nostd::get<std::string>(etw_span->GetAttributes().at("custom")), "unchanged");
  span->End();
  EXPECT_EQ(events.size(), 2u);
}
}  // namespace

TEST(ETWMessagePackTracer, EndSpanPreservesGeneratedTimeBetweenWrites)
{
  CheckEndSpanTime(false);
}

TEST(ETWMessagePackTracer, EndSpanPreservesSuppliedTimeBetweenWrites)
{
  CheckEndSpanTime(true);
}
