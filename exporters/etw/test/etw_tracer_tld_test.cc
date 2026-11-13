// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <windows.h>

#include <evntprov.h>

#include <gtest/gtest.h>

#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <thread>
#include <utility>
#include <vector>

namespace
{
struct CapturedEvent
{
  UCHAR opcode;
  std::vector<std::vector<uint8_t>> data;
};
std::vector<CapturedEvent> events;
std::function<void()> nested_write;

ULONG CaptureEventWriteTransfer(REGHANDLE,
                                const EVENT_DESCRIPTOR *descriptor,
                                LPCGUID,
                                LPCGUID,
                                ULONG count,
                                const EVENT_DATA_DESCRIPTOR *data)
{
  CapturedEvent event{descriptor->Opcode, {}};
  for (ULONG i = 0; i < count; ++i)
  {
    const auto *bytes = reinterpret_cast<const uint8_t *>(static_cast<uintptr_t>(data[i].Ptr));
    event.data.emplace_back(bytes, bytes + data[i].Size);
  }
  events.push_back(std::move(event));
  if (nested_write)
  {
    auto callback = std::move(nested_write);
    nested_write  = nullptr;
    callback();
  }
  return ERROR_SUCCESS;
}
}  // namespace

#define EventWriteTransfer CaptureEventWriteTransfer
#include "opentelemetry/exporters/etw/etw_tracer_exporter.h"
#undef EventWriteTransfer

using namespace OPENTELEMETRY_NAMESPACE;
namespace etw = OPENTELEMETRY_NAMESPACE::exporter::etw;

namespace
{
constexpr int64_t start_ns = 1709251200123456700;
constexpr int64_t end_ns   = 1709251200123468900;

class FixedSpan : public etw::Span
{
public:
  FixedSpan(etw::Tracer &owner, const trace::SpanContext &context)
      : etw::Span(owner, "operation", {}, std::make_unique<trace::SpanContext>(context))
  {
    start_time_ = common::SystemTimestamp(std::chrono::nanoseconds(start_ns));
  }
};

void CheckSpanEndEvents(bool tracking, bool ids = true)
{
  constexpr char provider_name[] = "TldSpanEndFields";
  etw::TracerProvider provider({{"enableTraceId", ids},
                                {"enableSpanId", ids},
                                {"enableActivityId", false},
                                {"enableRelatedActivityId", false},
                                {"enableActivityTracking", tracking}});
  auto tracer = provider.GetTracer(provider_name);
  auto &owner = *static_cast<etw::Tracer *>(tracer.get());
  ETWProvider writer;
  auto &handle                  = writer.open(provider_name, ETWProvider::ETW_MANIFEST);
  const uint8_t trace_bytes[16] = {1, 2, 3, 4};
  for (uint8_t scenario = 1; scenario <= 3; ++scenario)
  {
    const uint8_t span_bytes[8] = {scenario, 5, 6, 7};
    const trace::SpanContext context(trace::TraceId(trace_bytes), trace::SpanId(span_bytes),
                                     trace::TraceFlags(1), false);
    etw::Properties attributes;
    if (scenario == 1)
    {
      attributes["first-only"]       = "value";
      attributes[ETW_FIELD_SPAN_ID]  = "wrong-span-id";
      attributes[ETW_FIELD_TRACE_ID] = "wrong-trace-id";
    }
    else if (scenario == 2)
    {
      attributes[ETW_FIELD_SPAN_ID]  = etw::ToLowerBase16(context.span_id());
      attributes[ETW_FIELD_TRACE_ID] = etw::ToLowerBase16(context.trace_id());
    }
    FixedSpan span(owner, context);
    span.SetAttributes(attributes);
    const auto status = scenario == 1 ? trace::StatusCode::kError : trace::StatusCode::kOk;
    const std::string description = scenario == 1 ? std::string(4096, 'x') : std::string{};
    span.SetStatus(status, description);
    trace::EndSpanOptions end;
    end.end_steady_time = common::SteadyTimestamp(std::chrono::nanoseconds(end_ns));
    events.clear();
    span.End(end);
    const auto actual = events;
    span.End(end);
    EXPECT_EQ(events.size(), actual.size());
    EXPECT_EQ(span.GetAttributes().size(), attributes.size());

    events.clear();
    etw::Properties expected = attributes;
    expected[ETW_FIELD_NAME] = "operation";
    if (ids)
    {
      expected[ETW_FIELD_SPAN_ID]  = etw::ToLowerBase16(context.span_id());
      expected[ETW_FIELD_TRACE_ID] = etw::ToLowerBase16(context.trace_id());
    }
    if (tracking)
    {
      EXPECT_EQ(writer.write(handle, expected, nullptr, nullptr, 2, ETWProvider::ETW_MANIFEST),
                writer.STATUS_OK);
    }
    expected[ETW_FIELD_NAME]         = ETW_VALUE_SPAN;
    expected[ETW_FIELD_PAYLOAD_NAME] = "operation";
    expected[ETW_FIELD_STARTTIME]    = utils::formatUtcTimestampNsAsISO8601(start_ns);
#ifdef ETW_FIELD_ENDTTIME
    expected[ETW_FIELD_ENDTTIME] = utils::formatUtcTimestampNsAsISO8601(end_ns);
#endif
    expected[ETW_FIELD_DURATION]      = int64_t{end_ns - start_ns};
    expected[ETW_FIELD_SPAN_KIND]     = static_cast<uint32_t>(trace::SpanKind::kServer);
    expected[ETW_FIELD_SUCCESS]       = scenario == 1 ? "False" : "True";
    expected[ETW_FIELD_STATUSCODE]    = static_cast<uint32_t>(status);
    expected[ETW_FIELD_STATUSMESSAGE] = description;
    EXPECT_EQ(writer.write(handle, expected, nullptr, nullptr, 0, ETWProvider::ETW_MANIFEST),
              writer.STATUS_OK);
    ASSERT_EQ(actual.size(), events.size());
    for (size_t i = 0; i < actual.size(); ++i)
    {
      EXPECT_EQ(actual[i].opcode, events[i].opcode);
      EXPECT_EQ(actual[i].data, events[i].data) << "scenario " << static_cast<int>(scenario);
    }
  }
  writer.close(handle);
}
}  // namespace

TEST(ETWTldTracer, InlineFieldsMatchFreshEncodingWithoutStaleData)
{
  CheckSpanEndEvents(false, true);
}

TEST(ETWTldTracer, ActivityTrackingPreservesSeparateStopAndSpanFields)
{
  CheckSpanEndEvents(true, true);
}

TEST(ETWTldTracer, DisabledIdsMatchFreshEncoding)
{
  CheckSpanEndEvents(false, false);
  CheckSpanEndEvents(true, false);
}

TEST(ETWTldTracer, ReentrantEndSpanPreservesBothOuterWrites)
{
  for (bool tracking : {false, true})
  {
    etw::TracerProvider provider({{"enableActivityTracking", tracking}});
    auto tracer                   = provider.GetTracer("TldReentrantSpanEnd");
    auto &owner                   = *static_cast<etw::Tracer *>(tracer.get());
    const uint8_t trace_bytes[16] = {1};
    const uint8_t span_bytes[8]   = {2};
    const trace::SpanContext context(trace::TraceId(trace_bytes), trace::SpanId(span_bytes),
                                     trace::TraceFlags(1), false);
    trace::EndSpanOptions end;
    end.end_steady_time = common::SteadyTimestamp(std::chrono::nanoseconds(end_ns));
    auto emit           = [&](bool inner) {
      FixedSpan span(owner, context);
      etw::Properties attributes{{inner ? "inner" : "outer", std::string(1024, inner ? 'i' : 'o')}};
      span.SetAttributes(attributes);
      span.SetStatus(trace::StatusCode::kOk, "");
      span.End(end);
    };
    events.clear();
    emit(false);
    const auto expected = events;
    events.clear();
    nested_write = [&] { emit(true); };
    emit(false);
    ASSERT_EQ(events.size(), 2 * expected.size());
    EXPECT_EQ(events.front().data, expected.front().data);
    if (tracking)
    {
      EXPECT_EQ(events.back().data, expected.back().data);
    }
  }
}

TEST(ETWTldTracer, UnfinishedThreadLocalSpanExportsDuringTeardown)
{
  etw::TracerProvider provider({{"enableActivityTracking", false}});
  auto tracer = provider.GetTracer("TldThreadTeardown");
  events.clear();
  std::thread worker([&] {
    // Leave this span unfinished; the provider and tracer outlive worker teardown.
    thread_local nostd::shared_ptr<trace::Span> pending;
    pending       = tracer->StartSpan("thread-exit-span");
    auto finished = tracer->StartSpan("before-thread-exit");
    finished->End();
  });
  worker.join();
  ASSERT_EQ(events.size(), 2u);
  EXPECT_EQ(events[0].opcode, 0);
  EXPECT_EQ(events[1].opcode, 0);
}
