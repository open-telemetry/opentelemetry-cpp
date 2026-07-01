// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <chrono>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/instrumentationscope/instrumentation_scope.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/processor.h"
#include "opentelemetry/sdk/trace/recordable.h"
#include "opentelemetry/trace/span.h"
#include "opentelemetry/trace/span_context.h"
#include "opentelemetry/trace/span_id.h"
#include "opentelemetry/trace/trace_flags.h"
#include "opentelemetry/trace/trace_id.h"
#include "opentelemetry/trace/tracer.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace test_common
{

// A span processor that queues completed recordables into an internal buffer
// without exporting or destroying them, mirroring the batch processor's
// recording path. ForceFlush clears the buffer so benchmarks can drop recorded
// spans between iterations without measuring deallocation.
class BufferingSpanProcessor final : public sdk::trace::SpanProcessor
{
public:
  using Buffer = std::vector<std::unique_ptr<sdk::trace::Recordable>>;

  explicit BufferingSpanProcessor(std::unique_ptr<sdk::trace::SpanExporter> exporter)
      : exporter_(std::move(exporter))
  {}

  std::unique_ptr<sdk::trace::Recordable> MakeRecordable() noexcept override
  {
    return exporter_->MakeRecordable();
  }

  void OnStart(sdk::trace::Recordable &, const trace::SpanContext &) noexcept override {}

  void OnEnd(std::unique_ptr<sdk::trace::Recordable> &&span) noexcept override
  {
    buffer_.push_back(std::move(span));
  }

  bool ForceFlush(std::chrono::microseconds) noexcept override
  {
    buffer_.clear();
    return true;
  }

  bool Shutdown(std::chrono::microseconds) noexcept override { return true; }

private:
  std::unique_ptr<sdk::trace::SpanExporter> exporter_;
  Buffer buffer_;
};

inline const sdk::resource::Resource &TestResource()
{
  static const auto kResource =
      sdk::resource::Resource::Create({{"service.name", nostd::string_view{"benchmark_service"}},
                                       {"service.version", nostd::string_view{"1.0.0"}}});
  return kResource;
}

inline const sdk::instrumentationscope::InstrumentationScope &TestScope()
{
  static const auto kScope = sdk::instrumentationscope::InstrumentationScope::Create(
      "benchmark_scope", "1.0.0", "https://opentelemetry.io/schemas/1.24.0",
      {{"scope.source", "benchmark"}});
  return *kScope;
}

constexpr std::size_t kSpanAttributeLimit       = 128;
constexpr std::size_t kSpanEventLimit           = 128;
constexpr std::size_t kSpanLinkLimit            = 128;
constexpr std::size_t kSpanEventAttributesCount = 2;
constexpr std::size_t kSpanLinkAttributesCount  = 2;

// Pre-built unique attribute keys (up to the default limit) for attribute sweeps.
inline const std::vector<std::string> &AttributeKeys()
{
  static const std::vector<std::string> kAttributeKeys = []() {
    std::vector<std::string> result;
    result.reserve(kSpanAttributeLimit);
    for (std::size_t key_idx = 0; key_idx < kSpanAttributeLimit; ++key_idx)
    {
      result.push_back("attribute." + std::to_string(key_idx));
    }
    return result;
  }();
  return kAttributeKeys;
}

// A representative mix of attribute types used by the nominal span benchmark.
using SpanAttribute = std::pair<nostd::string_view, common::AttributeValue>;
inline const std::vector<SpanAttribute> &NominalSpanAttributes()
{
  static const std::vector<SpanAttribute> kNominalSpanAttributes = {
      {"benchmark.string.short", nostd::string_view{"abcdefgh"}},
      {"benchmark.string.medium", nostd::string_view{"abcdefghijklmnopqrstuvwxyz012345"}},
      {"benchmark.string.long",
       nostd::string_view{"abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz01"}},
      {"benchmark.int", common::AttributeValue{static_cast<int64_t>(1)}},
      {"benchmark.double", common::AttributeValue{3.14159265358979}},
      {"benchmark.bool", common::AttributeValue{true}},
  };
  return kNominalSpanAttributes;
}

inline std::vector<SpanAttribute> MakeAttributes(std::size_t count)
{
  if (count > kSpanAttributeLimit)
  {
    throw std::invalid_argument("count exceeds kSpanAttributeLimit");
  }

  const auto &keys = AttributeKeys();
  std::vector<SpanAttribute> attributes;
  attributes.reserve(count);
  for (std::size_t i = 0; i < count; ++i)
  {
    attributes.emplace_back(keys[i], nostd::string_view{"value-string-attribute"});
  }
  return attributes;
}

inline const std::vector<SpanAttribute> &SpanEventAttributes()
{
  static const std::vector<SpanAttribute> kSpanEventAttributes =
      MakeAttributes(kSpanEventAttributesCount);
  return kSpanEventAttributes;
}

inline const trace::SpanContext &TestSpanContext()
{
  static constexpr uint8_t kTraceId[trace::TraceId::kSize] = {2, 3, 4, 5, 6, 7, 8, 9,
                                                              2, 3, 4, 5, 6, 7, 8, 9};
  static constexpr uint8_t kSpanId[trace::SpanId::kSize]   = {9, 8, 7, 6, 5, 4, 3, 2};
  static const trace::SpanContext kTestSpanContext{trace::TraceId{kTraceId}, trace::SpanId{kSpanId},
                                                   trace::TraceFlags{trace::TraceFlags::kIsSampled},
                                                   true};
  return kTestSpanContext;
}

inline const std::vector<SpanAttribute> &SpanLinkAttributes()
{
  static const std::vector<SpanAttribute> kSpanLinkAttributes =
      MakeAttributes(kSpanLinkAttributesCount);
  return kSpanLinkAttributes;
}

using LinkEntry = std::pair<trace::SpanContext, std::vector<SpanAttribute>>;

inline std::vector<LinkEntry> MakeLinkEntries(std::size_t count)
{
  std::vector<LinkEntry> entries;
  entries.reserve(count);
  for (std::size_t i = 0; i < count; ++i)
  {
    entries.emplace_back(TestSpanContext(), SpanLinkAttributes());
  }
  return entries;
}

// Starts a span with no attributes, events, or links
inline nostd::shared_ptr<trace::Span> StartMinimalSpan(trace::Tracer &tracer)
{
  return tracer.StartSpan("benchmark_span");
}

// Starts a span with a representative mix of attributes
inline nostd::shared_ptr<trace::Span> StartNominalSpan(trace::Tracer &tracer)
{
  return tracer.StartSpan("benchmark_span", NominalSpanAttributes());
}

// Starts a span with pre-built attributes
inline nostd::shared_ptr<trace::Span> StartSpanWithAttributes(
    trace::Tracer &tracer,
    const std::vector<SpanAttribute> &attributes)
{
  return tracer.StartSpan("benchmark_span", attributes);
}

// Starts a span with pre-built links
inline nostd::shared_ptr<trace::Span> StartSpanWithLinks(trace::Tracer &tracer,
                                                         const std::vector<LinkEntry> &link_entries)
{
  return tracer.StartSpan("benchmark_span", nostd::span<const SpanAttribute>{}, link_entries);
}

// Starts a span and adds `num_events`
inline nostd::shared_ptr<trace::Span> StartSpanWithEvents(trace::Tracer &tracer,
                                                          std::size_t num_events)
{
  auto span = tracer.StartSpan("benchmark_span");
  for (std::size_t i = 0; i < num_events; ++i)
  {
    span->AddEvent("benchmark_event", SpanEventAttributes());
  }
  return span;
}

// Initialize all the static test data in singletons above. To be called in test fixture SetUp()
inline void InitializeSpanTestData()
{
  TestResource();
  TestScope();
  AttributeKeys();
  NominalSpanAttributes();
  SpanEventAttributes();
  SpanLinkAttributes();
  TestSpanContext();
}

}  // namespace test_common
OPENTELEMETRY_END_NAMESPACE
