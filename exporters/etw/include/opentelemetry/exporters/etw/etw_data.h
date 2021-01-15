#pragma once

#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <sstream>

#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/unique_ptr.h"

#include "opentelemetry/common/key_value_iterable_view.h"

#include "opentelemetry/trace/span.h"
#include "opentelemetry/trace/span_context_kv_iterable_view.h"
#include "opentelemetry/trace/span_id.h"
#include "opentelemetry/trace/trace_id.h"
#include "opentelemetry/trace/tracer_provider.h"

#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/recordable.h"
#include "opentelemetry/sdk/trace/span_data.h"

#include "opentelemetry/exporters/etw/etw_fields.h"
#include "opentelemetry/exporters/etw/etw_properties.h"
#include "opentelemetry/exporters/etw/etw_provider.h"
#include "opentelemetry/exporters/etw/utils.h"

namespace core  = opentelemetry::core;
namespace trace = opentelemetry::trace;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace ETW
{
class Span;

/**
 * @brief Utility template for Span.GetName()
 * @tparam T
 * @param t
 * @return
 */
template <class T>
std::string GetName(T &t)
{
  auto sV = t.GetName();
  return std::string(sV.data(), sV.length());
}

/**
 * @brief Utility template to convert SpanId or TraceId to hex.
 * @param id - value of SpanId or TraceId
 * @return Hexadecimal representation of Id as string.
 */
template <class T>
static inline std::string ToLowerBase16(const T &id)
{
  char buf[2 * T::kSize] = {0};
  id.ToLowerBase16(buf);
  return std::string(buf, sizeof(buf));
}

/**
 * @brief Tracer class that allows to send spans to ETW Provider.
 */
class Tracer : public trace::Tracer
{

  /**
   * @brief Parent provider of this Tracer
   */
  trace::TracerProvider &parent;

  /**
   * @brief ProviderId (Name or GUID)
   */
  std::string provId;

  /**
   * @brief Provider Handle
   */
  ETWProvider::Handle provHandle;

  /**
   * @brief Encoding (Manifest, MessagePack or XML)
   */
  ETWProvider::EventFormat encoding;

  /**
   * @brief ETWProvider is a singleton that aggregates all ETW writes.
   * @return
   */
  static ETWProvider &etwProvider()
  {
    static ETWProvider instance;  // C++11 magic static
    return instance;
  };

  /**
   * @brief Internal method that allows to populate Links to other Spans.
   * This is temporary implementation, that assumes the links are mentioned
   * comma-separated in their order of appearance.
   *
   * @param attributes
   * @param links
   */
  virtual void DecorateLinks(Properties &attributes,
                             const trace::SpanContextKeyValueIterable &links) const
  {
    // Links
    size_t idx = 0;
    std::string linksValue;
    links.ForEachKeyValue([&](trace::SpanContext ctx, const common::KeyValueIterable &) {
      if (!linksValue.empty())
      {
        linksValue += ',';
        linksValue += ToLowerBase16(ctx.span_id());
      }
      idx++;
      return true;
    });
    attributes[ETW_FIELD_SPAN_LINKS] = linksValue;
  };

#if 0
  // TODO: SpanContext is a temporary implementation that requires revisiting.
  // One option for ETW exporter is to respect the existing correlation fields:
  // - ActivityId         - as current Trace Id
  // - RelatedActivityId  - as parent Trace Id
  virtual trace::SpanContext GetDefaultContext() = 0;
#endif

  /**
   * @brief Allow our friendly ETW::Span to end itself on Tracer.
   * @param span
   * @param
   */
  virtual void EndSpan(const Span &span, const trace::EndSpanOptions & = {})
  {
    Properties evt;

    auto ctx               = (reinterpret_cast<const trace::Span *>(&span))->GetContext();
    evt[ETW_FIELD_NAME]    = GetName(span);
    evt[ETW_FIELD_TYPE]    = ETW_FIELD_SPAN_END;
    evt[ETW_FIELD_SPAN_ID] = ToLowerBase16(ctx.span_id());

    // TODO: check what EndSpanOptions should be supported for this exporter
    etwProvider().write(provHandle, evt, encoding);
  };

  friend class Span;

public:
  /**
   * @brief Tracer constructor
   * @param parent Parent TraceProvider
   * @param providerId ProviderId - Name or GUID
   * @param encoding ETW encoding format to use.
   */
  Tracer(trace::TracerProvider &parent,
         nostd::string_view providerId     = "",
         ETWProvider::EventFormat encoding = ETWProvider::EventFormat::ETW_MANIFEST)
      : trace::Tracer(),
        parent(parent),
        provId(providerId.data(), providerId.size()),
        encoding(encoding)
  {
#if defined(HAVE_MSGPACK) && !defined(HAVE_TLD)
    /* Fallback to MsgPack encoding if TraceLoggingDynamic feature gate is off */
    this->encoding = ETWProvider::EventFormat::ETW_MSGPACK;
#endif
    provHandle = etwProvider().open(provId, encoding);
  };

  /**
   * @brief Start Span
   * @param name Span name
   * @param attributes Span attributes
   * @param links Span links
   * @param options Span options
   * @return
   */
  nostd::shared_ptr<trace::Span> StartSpan(
      nostd::string_view name,
      const common::KeyValueIterable &attributes,
      const trace::SpanContextKeyValueIterable &links,
      const trace::StartSpanOptions &options = {}) noexcept override
  {
    nostd::shared_ptr<trace::Span> result = trace::to_span_ptr<Span, Tracer>(this, name, options);
    auto spanContext                      = result->GetContext();

    // Event properties
    Properties evt = attributes;

    // Decorate with additional standard fields
    evt[ETW_FIELD_NAME]     = name.data();
    evt[ETW_FIELD_TYPE]     = ETW_FIELD_SPAN_START;
    evt[ETW_FIELD_TRACE_ID] = ToLowerBase16(spanContext.trace_id());
    evt[ETW_FIELD_SPAN_ID]  = ToLowerBase16(spanContext.span_id());

    // Links
    DecorateLinks(evt, links);
    // NOTE: we neither allow adding attributes nor links after StartSpan.

    // TODO: add support for options
    // - options.kind
    // - options.parent
    // - options.start_steady_time
    // - options.start_system_time
    etwProvider().write(provHandle, evt, encoding);
    return result;
  };

  /**
   * @brief Force flush data to Tracer, spending up to given amount of microseconds to flush.
   * NOTE: this method has no effect for the realtime streaming Tracer.
   *
   * @param timeout Allow Tracer to drop data if timeout is reached
   * @return
   */
  void ForceFlushWithMicroseconds(uint64_t) noexcept override{};

  /**
   * @brief Close tracer, spending up to given amount of microseconds to flush and close.
   * NOTE: This method closes the current ETW Provider Handle.
   *
   * @param  timeout Allow Tracer to drop data if timeout is reached.
   * @return
   */
  void CloseWithMicroseconds(uint64_t) noexcept override { etwProvider().close(provHandle); };

  /**
   * @brief Add event data to span associated with tracer.
   * @param span Parent span.
   * @param name Event name.
   * @param timestamp Event timestamp.
   * @param attributes Event attributes.
   * @return
   */
  void AddEvent(trace::Span &span,
                nostd::string_view name,
                core::SystemTimestamp timestamp,
                const common::KeyValueIterable &attributes) noexcept
  {
    // TODO: respect originating timestamp
    (void)timestamp;

    // Event properties.
    // TODO: if we know that the incoming type is already `Properties`,
    // then we could avoid the memcpy by reusing the incoming attributes.
    // This would require non-const argument.
    Properties evt = attributes;

    evt[ETW_FIELD_NAME] = name.data();

    auto spanContext = span.GetContext();
    // Decorate with additional standard fields. This may get expensive
    // and should be configurable for traces that deduct their nested
    // structure based off built-in `ActivityId` and `RelatedActivityId`
    evt[ETW_FIELD_TRACE_ID] = ToLowerBase16(spanContext.trace_id());
    evt[ETW_FIELD_SPAN_ID]  = ToLowerBase16(spanContext.span_id());

    etwProvider().write(provHandle, evt, encoding);
  };

  /**
   * @brief Add event data to span associated with tracer.
   * @param span Span.
   * @param name Event name.
   * @param timestamp Event timestamp.
   * @return
   */
  void AddEvent(trace::Span &span,
                nostd::string_view name,
                core::SystemTimestamp timestamp) noexcept
  {
    AddEvent(span, name, timestamp, sdk::GetEmptyAttributes());
  };

  /**
   * @brief Add event data to span associated with tracer.
   * @param span Spab.
   * @param name Event name.
   */
  void AddEvent(trace::Span &span, nostd::string_view name)
  {
    AddEvent(span, name, std::chrono::system_clock::now(), sdk::GetEmptyAttributes());
  };

  /**
   * @brief Tracer destructor.
   */
  virtual ~Tracer() { CloseWithMicroseconds(0); };
};

/**
 * @brief ETW::Span allows to send event data to ETW listener.
 */
class Span : public trace::Span
{
protected:
  /**
   * @brief Parent (Owner) Tracer of this Span
   */
  Tracer &owner;

  /**
   * @brief Span name.
   */
  nostd::string_view name_;

  /**
   * @brief Attribute indicating that the span has ended.
   */
  std::atomic<bool> has_ended_{false};

  /**
   * @brief Attribute indicating that the span has started.
   */
  std::atomic<bool> has_started_{false};

public:
  /**
   * @brief Get Span Name.
   * @return Span Name.
   */
  nostd::string_view GetName() const { return name_; }

  /**
   * @brief Span constructor
   * @param owner Owner Tracer
   * @param name Span name
   * @param options Span options
   * @return
   */
  Span(Tracer &owner, nostd::string_view name, const trace::StartSpanOptions &options) noexcept
      : trace::Span(), owner(owner)
  {
    name_ = name;
    UNREFERENCED_PARAMETER(options);
  };

  /**
   * @brief Span Destructor
   */
  ~Span() { End(); }

  /**
   * @brief Add named event with no attributes.
   * @param name Event name.
   * @return
   */
  void AddEvent(nostd::string_view name) noexcept override { owner.AddEvent(*this, name); }

  /**
   * @brief Add named event with custom timestamp.
   * @param name
   * @param timestamp
   * @return
   */
  void AddEvent(nostd::string_view name, core::SystemTimestamp timestamp) noexcept override
  {
    owner.AddEvent(*this, name, timestamp);
  }

  /**
   * @brief Add named event with custom timestamp and attributes.
   * @param name Event name.
   * @param timestamp Event timestamp.
   * @param attributes Event attributes.
   * @return
   */
  void AddEvent(nostd::string_view name,
                core::SystemTimestamp timestamp,
                const common::KeyValueIterable &attributes) noexcept override
  {
    owner.AddEvent(*this, name, timestamp, attributes);
  }

  /**
   * @brief Set Span status
   * @param code Span status code.
   * @param description Span description.
   * @return
   */
  void SetStatus(trace::CanonicalCode code, nostd::string_view description) noexcept override
  {
    // TODO: not implemented
    UNREFERENCED_PARAMETER(code);
    UNREFERENCED_PARAMETER(description);
  };

  /**
   * @brief Sets an attribute on the Span. If the Span previously contained a mapping
   * for the key, the old value is replaced.
   *
   * @param key
   * @param value
   * @return
   */
  void SetAttribute(nostd::string_view key, const common::AttributeValue &value) noexcept override
  {
    // TODO: not implemented
    UNREFERENCED_PARAMETER(key);
    UNREFERENCED_PARAMETER(value);
  };

  /**
   * @brief Update Span name.
   *
   * NOTE: this method is a no-op for streaming implementation.
   * We cannot change the Span name after it started streaming.
   *
   * @param name
   * @return
   */
  void UpdateName(nostd::string_view) noexcept override
  {
    // We can't do that!
    // name_ = name;
  }

  /**
   * @brief End Span.
   * @param EndSpanOptions
   * @return
   */
  void End(const trace::EndSpanOptions &options = {}) noexcept override
  {
    if (!has_ended_.exchange(true))
    {
      owner.EndSpan(*this, options);
    }
  }

  /**
   * @brief Obtain SpanContext
   * @return
   */
  trace::SpanContext GetContext() const noexcept override
  {
    // TODO: not implemented
    static trace::SpanContext nullContext{trace::SpanContext::GetInvalid()};
    return nullContext;
  }

  /**
   * @brief Check if Span is recording data.
   * @return
   */
  bool IsRecording() const noexcept override
  {
    // For streaming implementation this should return the state of ETW Listener.
    // In certain unprivileged environments, ex. containers, it is impossible
    // to determine if a listener is registered. Thus, we always return true.
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
  ///

  /**
   * @brief Obtain ETW Tracer.
   * @param name ProviderId (instrumentation name) - Name or GUID
   *
   * @param args Additional arguments that controls `codec` of the provider.
   * Possible values are:
   * - "ETW"            - 'classic' Trace Logging Dynamic manifest ETW events.
   * - "MSGPACK"        - MessagePack-encoded binary payload ETW events.
   * - "XML"            - XML events (reserved for future use)
   * @return
   */
  nostd::shared_ptr<trace::Tracer> GetTracer(nostd::string_view name,
                                             nostd::string_view args = "") override
  {
#if defined(HAVE_MSGPACK)
    // Prefer MsgPack over ETW by default
    ETWProvider::EventFormat evtFmt = ETWProvider::EventFormat::ETW_MSGPACK;
#else
    // Fallback to ETW TraceLoggingDynamic if MsgPack support is not compiled in
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

}  // namespace ETW
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
