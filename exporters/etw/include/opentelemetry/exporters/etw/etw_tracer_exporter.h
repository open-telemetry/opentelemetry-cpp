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

#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/unique_ptr.h"

#include "opentelemetry/common/key_value_iterable_view.h"
#include "opentelemetry/trace/span.h"
#include "opentelemetry/trace/span_id.h"
#include "opentelemetry/trace/trace_id.h"
#include "opentelemetry/trace/tracer_provider.h"
#include "opentelemetry/trace/span_context_kv_iterable_view.h"

#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/recordable.h"
#include "opentelemetry/sdk/trace/span_data.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <unordered_map>

#include "opentelemetry/exporters/etw/etw_provider_exporter.h"

#include "opentelemetry/exporters/etw/utils.h"

namespace core  = opentelemetry::core;
namespace trace = opentelemetry::trace;

OPENTELEMETRY_BEGIN_NAMESPACE

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
  /// Encoding (Manifest, MessagePack or XML)
  /// </summary>
  ETWProvider::EventFormat encoding;

  /// <summary>
  /// ETWProvider is a singleton that aggregates all ETW writes.
  /// </summary>
  /// <returns></returns>
  static ETWProvider &etwProvider()
  {
    static ETWProvider instance;  // C++11 magic static
    return instance;
  };

public:
  /// <summary>
  /// Tracer constructor
  /// </summary>
  /// <param name="parent">Parent TraceProvider</param>
  /// <param name="providerId">providerId</param>
  /// <returns>Tracer instance</returns>
  Tracer(trace::TracerProvider &parent,
         nostd::string_view providerId     = "",
         ETWProvider::EventFormat encoding = ETWProvider::EventFormat::ETW_MANIFEST)
      : trace::Tracer(),
        parent(parent),
        provId(providerId.data(), providerId.size()),
        encoding(encoding)
  {
    provHandle = etwProvider().open(provId, encoding);
  };

  /// <summary>
  /// Start Span
  /// </summary>
  /// <param name="name">Span name</param>
  /// <param name="options">Span options</param>
  /// <returns>Span</returns>
  virtual nostd::shared_ptr<trace::Span> StartSpan(
      nostd::string_view name,
      const common::KeyValueIterable &attributes,
      const trace::SpanContextKeyValueIterable &links,
      const trace::StartSpanOptions &options = {}) noexcept override
  {
    // TODO: support attributes
    UNREFERENCED_PARAMETER(attributes);
    UNREFERENCED_PARAMETER(links);
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
  virtual void CloseWithMicroseconds(uint64_t) noexcept override
  {
    etwProvider().close(provHandle);
  };

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
                const common::KeyValueIterable &attributes) noexcept
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

#ifdef HAVE_TLD
    if (encoding == ETWProvider::ETW_MANIFEST)
    {
      etwProvider().writeTld(provHandle, m);
      return;
    }
#endif

#ifdef HAVE_MSGPACK
    if (encoding == ETWProvider::ETW_MSGPACK)
    {
      etwProvider().writeMsgPack(provHandle, m);
      return;
    }
#endif
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
    AddEvent(span, name, timestamp, common::NullKeyValueIterable());
  };

  /// <summary>
  /// Add event data to span associated with tracer
  /// </summary>
  /// <param name="span"></param>
  /// <param name="name"></param>
  void AddEvent(Span &span, nostd::string_view name)
  {
    AddEvent(span, name, std::chrono::system_clock::now(), common::NullKeyValueIterable());
  };

  virtual ~Tracer() { CloseWithMicroseconds(0); };
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
                const common::KeyValueIterable &attributes) noexcept override
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

  virtual trace::SpanContext GetContext() const noexcept
  {
    // TODO: not implemented
    static trace::SpanContext nullContext;
    return nullContext;
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

  virtual void SetToken(nostd::unique_ptr<context::Token> &&token) noexcept
  {
    // TODO: not implemented
    UNREFERENCED_PARAMETER(token);
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
    // TODO: describe possible args. Currently supported:
    // "MSGPACK"        - use MessagePack
    // "XML"            - use XML
    // "ETW"            - use 'classic' Trace Logging Dynamic manifest ETW event
    //
#if defined(HAVE_NO_TLD) && defined(HAVE_MSGPACK)
    ETWProvider::EventFormat evtFmt = ETWProvider::EventFormat::ETW_MSGPACK;
#else
    ETWProvider::EventFormat evtFmt = ETWProvider::EventFormat::ETW_MANIFEST;
#endif

#pragma warning(push)
#pragma warning(disable : 4307) /* Integral constant overflow - OK while computing hash */
    auto h = utils::hashCode(args.data());
    switch (h)
    {
      case CONST_HASHCODE(MSGPACK):
        // nobrk
      case CONST_HASHCODE(MsgPack):
        // nobrk
      case CONST_HASHCODE(MessagePack):
        evtFmt = ETWProvider::EventFormat::ETW_MSGPACK;
        break;

      case CONST_HASHCODE(XML):
        // nobrk
      case CONST_HASHCODE(xml):
        evtFmt = ETWProvider::EventFormat::ETW_XML;
        break;

      case CONST_HASHCODE(TLD):
        // nobrk
      case CONST_HASHCODE(tld):
        // nobrk
        evtFmt = ETWProvider::EventFormat::ETW_MANIFEST;
        break;

      default:
        break;
    }
#pragma warning(pop)
    return nostd::shared_ptr<trace::Tracer>{new (std::nothrow) Tracer(*this, name, evtFmt)};
  }
};

class ETWTracerExporter final : public opentelemetry::sdk::trace::SpanExporter
{
public:
  /**
   * @param providerName
   * @param eventName
   */
  ETWTracerExporter(std::string providerName, std::string eventName): providerName_(providerName), eventName_(eventName) 
  {
  }

  /**
   * @return Returns a unique pointer to an empty recordable object
   */
  std::unique_ptr<sdk::trace::Recordable> MakeRecordable() noexcept override
  {
    // TODO: Discuss how we want to make the Recordable
    //return std::unique_ptr<sdk::trace::Recordable>(new TracerProvider());
  }

  /**
   * @param recordables a required span containing unique pointers to the data
   * to add to the ETWTracerExporter
   * @return Returns the result of the operation
   */
  sdk::trace::ExportResult Export(
      const nostd::span<std::unique_ptr<sdk::trace::Recordable>> &recordables) noexcept override
  {
    for (auto &recordable : recordables)
    {
      auto tp = std::unique_ptr<TracerProvider>(
          dynamic_cast<TracerProvider *>(recordable.release()));
      if (tp != nullptr)
      {
        auto tracer = tp->GetTracer(providerName_);
        auto span   = tracer->StartSpan("MySpan");

        // do whatever is needed
      }
    }

    return sdk::trace::ExportResult::kSuccess;
  }

  /**
   * @param timeout an optional value containing the timeout of the exporter
   * note: passing custom timeout values is not currently supported for this exporter
   */
  void Shutdown(
      std::chrono::microseconds timeout = std::chrono::microseconds(0)) noexcept override{};

private:
  std::string providerName_;
  std::string eventName_;
};

} // namespace ETW

OPENTELEMETRY_END_NAMESPACE
