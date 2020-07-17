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
#pragma once

#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include <mutex>

#include <opentelemetry/nostd/nostd.h>
#include <opentelemetry/trace/key_value_iterable_view.h>
#include <opentelemetry/trace/span.h>
#include <opentelemetry/trace/span_id.h>
#include <opentelemetry/trace/trace_id.h>
#include <opentelemetry/trace/tracer_provider.h>

#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <unordered_map>

#include "ETWProvider.hpp"

namespace core  = opentelemetry::core;
namespace trace = opentelemetry::trace;

OPENTELEMETRY_BEGIN_NAMESPACE

/// <summary>
/// ETW namespace provides no-exporter header-only implementation of ETW Trace Provider
/// </summary>
namespace ETW
{

class Span;

/// <summary>
/// stream::Tracer class that allows to send spans to ETW
/// </summary>
class Tracer : public trace::Tracer
{
  /// <summary>
  /// Parent provider of this Tracer
  /// </summary>
  trace::TracerProvider &parent;

  /// <summary>
  /// Provider Id (Name or GUID)
  /// </summary>
  std::string provId;

  /// <summary>
  /// Provider Handle
  /// </summary>
  ETWProvider::Handle provHandle;

  /// <summary>
  /// ETWProvider is a singleton that aggregates all ETW writes.
  /// </summary>
  /// <returns></returns>
  static ETWProvider &etwProvider()
  {
    static ETWProvider instance; // C++11 magic static
    return instance;
  };

public:
  /// <summary>
  /// Tracer constructor
  /// </summary>
  /// <param name="parent">Parent TraceProvider</param>
  /// <param name="providerId">providerId</param>
  /// <returns>Tracer instance</returns>
  Tracer(trace::TracerProvider &parent, nostd::string_view providerId = "")
      : trace::Tracer(), parent(parent), provId(providerId.data(), providerId.size())
  {
    provHandle = etwProvider().open(provId);
  };

  /// <summary>
  /// Start Span
  /// </summary>
  /// <param name="name">Span name</param>
  /// <param name="options">Span options</param>
  /// <returns>Span</returns>
  virtual nostd::unique_ptr<trace::Span> StartSpan(
      nostd::string_view name,
      const trace::KeyValueIterable &attributes,
      const trace::StartSpanOptions &options = {}) noexcept override
  {
    // TODO: support attributes
    return trace::to_span_ptr<Span, Tracer>(this, name, options);
  };

  /// <summary>
  /// Force flush data to Tracer, spending up to given amount of microseconds to flush.
  /// </summary>
  /// <param name="timeout">Allow Tracer to drop data if timeout is reached</param>
  /// <returns>void</returns>
  virtual void ForceFlushWithMicroseconds(uint64_t) noexcept override{};

  /// <summary>
  /// Close tracer, spending up to given amount of microseconds to flush and close.
  /// </summary>
  /// <param name="timeout">Allow Tracer to drop data if timeout is reached</param>
  /// <returns></returns>
  virtual void CloseWithMicroseconds(uint64_t) noexcept override{};

  /// <summary>
  /// Add event data to span associated with tracer
  /// </summary>
  /// <param name="span"></param>
  /// <param name="name"></param>
  /// <param name="timestamp"></param>
  /// <param name="attributes"></param>
  /// <returns></returns>
  void AddEvent(Span &span,
                nostd::string_view name,
                core::SystemTimestamp timestamp,
                const trace::KeyValueIterable &attributes) noexcept
  {
    // TODO: associate events with span
    (void)span;
    // TODO: respect original timestamp
    (void)timestamp;

    // Unfortunately we copy right now since we don't have the knowledge of original map object :-(
    std::map<nostd::string_view, common::AttributeValue> m;
    attributes.ForEachKeyValue([&](nostd::string_view key, common::AttributeValue value) noexcept {
      m[key] = value;
      return true;
    });
    m["name"] = name.data();
    etwProvider().write(provHandle, m);
  };

  /// <summary>
  /// Add event data to span associated with tracer
  /// </summary>
  /// <param name="span"></param>
  /// <param name="name"></param>
  /// <param name="timestamp"></param>
  /// <returns></returns>
  void AddEvent(Span &span, nostd::string_view name, core::SystemTimestamp timestamp) noexcept
  {
    AddEvent(span, name, timestamp, trace::NullKeyValueIterable());
  };

  /// <summary>
  /// Add event data to span associated with tracer
  /// </summary>
  /// <param name="span"></param>
  /// <param name="name"></param>
  void AddEvent(Span &span, nostd::string_view name)
  {
    AddEvent(span, name, std::chrono::system_clock::now(), trace::NullKeyValueIterable());
  };

  virtual ~Tracer() { etwProvider().close(provHandle); };
};

/// <summary>
/// stream::Span allows to send event data to stream
/// </summary>
class Span : public trace::Span
{

protected:
  /// <summary>
  /// Parent (Owner) Tracer of this Span
  /// </summary>
  Tracer &owner;

public:
  /// <summary>
  /// Span constructor
  /// </summary>
  /// <param name="owner">Owner Tracer</param>
  /// <param name="name">Span name</param>
  /// <param name="options">Span options</param>
  /// <returns>Span</returns>
  Span(Tracer &owner, nostd::string_view name, const trace::StartSpanOptions &options) noexcept
      : trace::Span(), owner(owner)
  {
    UNREFERENCED_PARAMETER(name);
    UNREFERENCED_PARAMETER(options);
  };

  ~Span() { End(); }

  /// <summary>
  /// Add named event with no attributes
  /// </summary>
  /// <param name="name"></param>
  /// <returns></returns>
  virtual void AddEvent(nostd::string_view name) noexcept override { owner.AddEvent(*this, name); }

  /// <summary>
  /// Add named event with custom timestamp
  /// </summary>
  /// <param name="name"></param>
  /// <param name="timestamp"></param>
  /// <returns></returns>
  virtual void AddEvent(nostd::string_view name, core::SystemTimestamp timestamp) noexcept override
  {
    owner.AddEvent(*this, name, timestamp);
  }

  /// <summary>
  /// Add named event with custom timestamp and attributes
  /// </summary>
  /// <param name="name"></param>
  /// <param name="timestamp"></param>
  /// <param name="attributes"></param>
  /// <returns></returns>
  void AddEvent(nostd::string_view name,
                core::SystemTimestamp timestamp,
                const trace::KeyValueIterable &attributes) noexcept override
  {
    owner.AddEvent(*this, name, timestamp, attributes);
  }

  /// <summary>
  /// Set Span status
  /// </summary>
  /// <param name="code"></param>
  /// <param name="description"></param>
  /// <returns></returns>
  virtual void SetStatus(trace::CanonicalCode code,
                         nostd::string_view description) noexcept override
  {
    // TODO: not implemented
    UNREFERENCED_PARAMETER(code);
    UNREFERENCED_PARAMETER(description);
  };

  // Sets an attribute on the Span. If the Span previously contained a mapping for
  // the key, the old value is replaced.
  virtual void SetAttribute(nostd::string_view key,
                            const common::AttributeValue &value) noexcept override
  {
    // TODO: not implemented
    UNREFERENCED_PARAMETER(key);
    UNREFERENCED_PARAMETER(value);
  };

  /// <summary>
  /// Update Span name
  /// </summary>
  /// <param name="name"></param>
  /// <returns></returns>
  virtual void UpdateName(nostd::string_view name) noexcept override
  {
    // TODO: not implemented
    UNREFERENCED_PARAMETER(name);
  }

  /// <summary>
  /// End Span
  /// </summary>
  /// <returns></returns>
  virtual void End(const trace::EndSpanOptions & = {}) noexcept override
  {
    // TODO: signal this to owner
  }

  /// <summary>
  /// Check if Span is recording data
  /// </summary>
  /// <returns></returns>
  virtual bool IsRecording() const noexcept override
  {
    // TODO: not implemented
    return true;
  }

  /// <summary>
  /// Get Owner tracer of this Span
  /// </summary>
  /// <returns></returns>
  trace::Tracer &tracer() const noexcept { return this->owner; };
};

/// <summary>
/// stream::TraceProvider
/// </summary>
class TracerProvider : public trace::TracerProvider
{
public:
  /// <summary>
  /// Obtain a Tracer of given type (name) and supply extra argument arg2 to it.
  /// </summary>
  /// <param name="name">Tracer Type</param>
  /// <param name="args">Tracer arguments</param>
  /// <returns></returns>
  virtual nostd::shared_ptr<trace::Tracer> GetTracer(nostd::string_view name,
                                                     nostd::string_view args = "")
  {
    UNREFERENCED_PARAMETER(args);
    return nostd::shared_ptr<trace::Tracer>{new (std::nothrow) Tracer(*this, name)};
  }
};

}  // namespace ETW
OPENTELEMETRY_END_NAMESPACE
