// Copyright 2021, OpenTelemetry Authors
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

#include <algorithm>
#include <atomic>

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
#include <vector>

#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/unique_ptr.h"
#include "opentelemetry/nostd/variant.h"

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

/**
 * @brief TracerProvider Options passed via SDK API.
 */
using TracerProviderOptions =
    std::map<std::string, nostd::variant<std::string, uint64_t, float, bool>>;

/**
 * @brief TracerProvider runtime configuration class. Internal representation
 * of TracerProviderOptions used by various components of SDK.
 */
typedef struct
{
  bool enableTraceId;            // Set `TraceId` on ETW events
  bool enableSpanId;             // Set `SpanId` on ETW events
  bool enableActivityId;         // Assign `SpanId` to `ActivityId`
  bool enableRelatedActivityId;  // Assign parent `SpanId` to `RelatedActivityId`
  bool enableAutoParent;         // Start new spans as children of current active span
} TracerProviderConfiguration;

/**
 * @brief Helper template to convert a variant value from TracerProviderOptions to
 * TracerProviderConfiguration
 *
 * @param options           TracerProviderOptions passed on API surface
 * @param key               Option name
 * @param value             Reference to destination value
 * @param defaultValue      Default value if option is not supplied
 */
template <typename T>
static inline void GetOption(const TracerProviderOptions &options,
                             const char *key,
                             T &value,
                             T defaultValue)
{
  auto it = options.find(key);
  if (it != options.end())
  {
    auto val = it->second;
    value    = nostd::get<T>(val);
  }
  else
  {
    value = defaultValue;
  }
}

class Span;
class TracerProvider;

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
 * @brief Utility template for forward-declaration of ETW::TracerProvider._config
 *
 * @tparam T    ETW::TracerProvider
 * @param t     ETW::TracerProvider ref
 * @return      TracerProviderConfiguration ref
 */
template <class T>
TracerProviderConfiguration &GetConfiguration(T &t)
{
  return t.config_;
}

/**
 * @brief Utility method to convert SppanContext.span_id() (8 byte) to ActivityId GUID (16 bytes)
 * @param span OpenTelemetry Span pointer
 * @return GUID struct containing 8-bytes of SpanId + 8 NUL bytes.
 */
static inline bool CopySpanIdToActivityId(const trace::SpanContext &spanContext, GUID &outGuid)
{
  memset(&outGuid, 0, sizeof(outGuid));
  if (!spanContext.IsValid())
  {
    return false;
  }
  auto spanId = spanContext.span_id().Id().data();
  std::copy(spanId, spanId + 8, reinterpret_cast<uint8_t *>(&outGuid));
  return true;
};

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
  ETW::TracerProvider &tracerProvider_;

  /**
   * @brief ProviderId (Name or GUID)
   */
  std::string provId;

  /**
   * @brief Encoding (Manifest, MessagePack or XML)
   */
  ETWProvider::EventFormat encoding;

  /**
   * @brief Provider Handle
   */
  ETWProvider::Handle &provHandle;

  trace::TraceId traceId_;

  std::atomic<bool> isClosed_{true};

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
   * Span links are in hexadecimal representation, comma-separated in their
   * order of appearance.
   *
   * @param attributes
   * @param links
   */
  virtual void DecorateLinks(Properties &attributes,
                             const trace::SpanContextKeyValueIterable &links) const
  {
    // Add `SpanLinks` attribute if the list is not empty
    if (links.size())
    {
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
    }
  };

  /**
   * @brief Allow our friendly ETW::Span to end itself on Tracer.
   * @param span
   * @param
   */
  virtual void EndSpan(const Span &span,
                       const trace::Span *parentSpan = nullptr,
                       const trace::EndSpanOptions & = {})
  {
    const auto &cfg             = GetConfiguration(tracerProvider_);
    const trace::Span &spanBase = reinterpret_cast<const trace::Span &>(span);
    auto spanContext            = spanBase.GetContext();

    Properties evt;
    evt[ETW_FIELD_NAME] = GetName(span);

    if (cfg.enableSpanId)
    {
      evt[ETW_FIELD_SPAN_ID] = ToLowerBase16(spanContext.span_id());
    }

    if (cfg.enableTraceId)
    {
      evt[ETW_FIELD_TRACE_ID] = ToLowerBase16(spanContext.trace_id());
    }

    // Populate ActivityId if enabled
    GUID ActivityId;
    LPGUID ActivityIdPtr = nullptr;
    if (cfg.enableActivityId)
    {
      if (CopySpanIdToActivityId(spanBase.GetContext(), ActivityId))
      {
        ActivityIdPtr = &ActivityId;
      }
    }

    // Populate RelatedActivityId if enabled
    GUID RelatedActivityId;
    LPGUID RelatedActivityIdPtr = nullptr;
    if (cfg.enableRelatedActivityId)
    {
      if (parentSpan != nullptr)
      {
        if (CopySpanIdToActivityId(parentSpan->GetContext(), RelatedActivityId))
        {
          RelatedActivityIdPtr = &RelatedActivityId;
        }
      }
    }

    // TODO: check what EndSpanOptions should be supported for this exporter.
    // The only option available currently (end_steady_time) does not apply.
    etwProvider().write(provHandle, evt, ActivityIdPtr, RelatedActivityIdPtr, 2, encoding);

    {
      // Atomically remove the span from list of spans
      const std::lock_guard<std::mutex> lock(scopes_mutex_);
      auto spanId = ToLowerBase16(spanBase.GetContext().span_id());
      scopes_.erase(spanId);
    }
  };

  const trace::TraceId &trace_id() { return traceId_; };

  friend class Span;

  std::mutex scopes_mutex_;  // protects scopes_
  std::map<std::string, nostd::unique_ptr<trace::Scope>> scopes_;

  /**
   * @brief Init a reference to ETW::ProviderHandle
   * @return Provider Handle
   */
  ETWProvider::Handle &initProvHandle()
  {
#if defined(HAVE_MSGPACK) && !defined(HAVE_TLD)
    /* Fallback to MsgPack encoding if TraceLoggingDynamic feature gate is off */
    encoding = ETWProvider::EventFormat::ETW_MSGPACK;
#endif
    isClosed_ = false;
    return etwProvider().open(provId, encoding);
  }

public:
  /**
   * @brief Tracer constructor
   * @param parent Parent TraceProvider
   * @param providerId ProviderId - Name or GUID
   * @param encoding ETW encoding format to use.
   */
  Tracer(ETW::TracerProvider &parent,
         nostd::string_view providerId     = "",
         ETWProvider::EventFormat encoding = ETWProvider::EventFormat::ETW_MANIFEST)
      : trace::Tracer(),
        tracerProvider_(parent),
        provId(providerId.data(), providerId.size()),
        encoding(encoding),
        provHandle(initProvHandle())
  {
    // Generate random GUID
    GUID trace_id;
    CoCreateGuid(&trace_id);
    // Populate TraceId of the Tracer with that random GUID
    const auto *traceIdBytes = reinterpret_cast<const uint8_t *>(std::addressof(trace_id));
    traceId_                 = trace::TraceId(traceIdBytes);
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
    const auto &cfg = GetConfiguration(tracerProvider_);

    // Parent Context:
    // - either use current span
    // - or attach to parent SpanContext specified in options
    const auto parentContext =
        (options.parent.IsValid()) ? options.parent : GetCurrentSpan()->GetContext();

    // Copy Span attributes to event Payload
    Properties evt = attributes;

    // Populate Etw.RelatedActivityId at envelope level if enabled
    GUID RelatedActivityId;
    LPCGUID RelatedActivityIdPtr = nullptr;
    if (cfg.enableAutoParent)
    {
      if (cfg.enableRelatedActivityId)
      {
        if (CopySpanIdToActivityId(parentContext, RelatedActivityId))
        {
          RelatedActivityIdPtr = &RelatedActivityId;
        }
      }
    }

    nostd::shared_ptr<trace::Span> result = trace::to_span_ptr<Span, Tracer>(this, name, options);
    auto spanContext                      = result->GetContext();

    // Decorate with additional standard fields
    std::string eventName = name.data();

    // Populate Etw.EventName attribute at envelope level
    evt[ETW_FIELD_NAME] = eventName;

    // Populate Payload["SpanId"] attribute
    // Populate Payload["ParentSpanId"] attribute if parent Span is valid
    if (cfg.enableSpanId)
    {
      if (parentContext.IsValid())
      {
        evt[ETW_FIELD_SPAN_PARENTID] = ToLowerBase16(parentContext.span_id());
      }
      evt[ETW_FIELD_SPAN_ID] = ToLowerBase16(spanContext.span_id());
    }

    // Populate Etw.Payload["TraceId"] attribute
    if (cfg.enableTraceId)
    {
      evt[ETW_FIELD_TRACE_ID] = ToLowerBase16(spanContext.trace_id());
    }

    // Populate Etw.ActivityId at envelope level if enabled
    GUID ActivityId;
    LPCGUID ActivityIdPtr = nullptr;
    if (cfg.enableActivityId)
    {
      if (CopySpanIdToActivityId(result.get()->GetContext(), ActivityId))
      {
        ActivityIdPtr = &ActivityId;
      }
    }

    // Links
    DecorateLinks(evt, links);

    // TODO: add support for options that are presently ignored :
    // - options.kind
    // - options.start_steady_time
    // - options.start_system_time
    etwProvider().write(provHandle, evt, ActivityIdPtr, RelatedActivityIdPtr, 1, encoding);

    {
      const std::lock_guard<std::mutex> lock(scopes_mutex_);
      // Use span_id as index
      scopes_[ToLowerBase16(result->GetContext().span_id())] = WithActiveSpan(result);
    }

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
   * NOTE: This method decrements the reference count on current ETW Provider Handle and
   * closes it if reference count on that provider handle is zero.
   *
   * @param  timeout Allow Tracer to drop data if timeout is reached.
   * @return
   */
  void CloseWithMicroseconds(uint64_t) noexcept override
  {
    // Close once only
    if (!isClosed_.exchange(true))
    {
      etwProvider().close(provHandle);
    }
  };

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
    // TODO: respect originating timestamp. Do we need to reserve
    // a special 'Timestamp' field or is it an overkill? The delta
    // between when `AddEvent` API is called and when ETW layer
    // timestamp is appended is nanos- to micros-, thus handling
    // the explicitly provided timestamp is only necessary in case
    // if a process wants to submit back-dated or future-dated
    // timestamp. Unless there is a strong ask from any ETW customer
    // to have it, this feature (custom timestamp) remains unimplemented.
    (void)timestamp;

    const auto &cfg = GetConfiguration(tracerProvider_);

    // OPTIMIZATION OPPORTUNITY: Event properties assigned from attributes.
    // If we know that the parameter is of non-const container type `Properties`,
    // then we can append more to container and avoid the memcpy entirely.
    Properties evt = attributes;

    evt[ETW_FIELD_NAME] = name.data();

    const auto &spanContext = span.GetContext();
    if (cfg.enableSpanId)
    {
      evt[ETW_FIELD_SPAN_ID] = ToLowerBase16(spanContext.span_id());
    }

    if (cfg.enableTraceId)
    {
      evt[ETW_FIELD_TRACE_ID] = ToLowerBase16(spanContext.trace_id());
    }

    LPGUID ActivityIdPtr = nullptr;
    GUID ActivityId;
    if (cfg.enableActivityId)
    {
      CopySpanIdToActivityId(spanContext, ActivityId);
      ActivityIdPtr = &ActivityId;
    }

    etwProvider().write(provHandle, evt, ActivityIdPtr, nullptr, 0, encoding);
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
   * @brief Owner Tracer of this Span
   */
  Tracer &owner_;

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

  /**
   * @brief Parent Span of this nested Span (optional)
   */
  Span *parent_{nullptr};

  /**
   * @brief Get Parent Span of this nested Span.
   * @return Pointer to Parent or nullptr if no Parent.
   */
  Span *GetParent() const { return parent_; }

  trace::SpanContext context_;

  const trace::SpanContext CreateContext()
  {
    GUID activity_id;
    // Generate random GUID
    CoCreateGuid(&activity_id);
    const auto *activityIdBytes = reinterpret_cast<const uint8_t *>(std::addressof(activity_id));
    // Populate SpanId with that GUID
    const trace::SpanId spanId(activityIdBytes);
    // Inherit trace_id from Tracer
    const trace::TraceId traceId{owner_.trace_id()};
    // TODO: TraceFlags are not supported by ETW exporter.
    const trace::TraceFlags flags{0};
    // TODO: Remote parent is not supported by ETW exporter.
    const bool hasRemoteParent = false;
    return trace::SpanContext{traceId, spanId, flags, hasRemoteParent};
  }

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
   * @param parent Parent Span (optional)
   * @return
   */
  Span(Tracer &owner,
       nostd::string_view name,
       const trace::StartSpanOptions &options,
       Span *parent = nullptr) noexcept
      : trace::Span(), owner_(owner), parent_(parent), context_(CreateContext())
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
  void AddEvent(nostd::string_view name) noexcept override { owner_.AddEvent(*this, name); }

  /**
   * @brief Add named event with custom timestamp.
   * @param name
   * @param timestamp
   * @return
   */
  void AddEvent(nostd::string_view name, core::SystemTimestamp timestamp) noexcept override
  {
    owner_.AddEvent(*this, name, timestamp);
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
    owner_.AddEvent(*this, name, timestamp, attributes);
  }

  /**
   * @brief Set Span status
   * @param code Span status code.
   * @param description Span description.
   * @return
   */
  void SetStatus(trace::StatusCode code, nostd::string_view description) noexcept override
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
      owner_.EndSpan(*this, parent_, options);
    }
  }

  /**
   * @brief Obtain SpanContext
   * @return
   */
  trace::SpanContext GetContext() const noexcept override { return context_; }

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
  trace::Tracer &tracer() const noexcept { return this->owner_; };
};

/**
 * @brief ETW TracerProvider
 */
class TracerProvider : public trace::TracerProvider
{
public:
  /**
   * @brief TracerProvider options supplied during initialization.
   */
  TracerProviderConfiguration config_;

  /**
   * @brief Construct instance of TracerProvider with given options
   * @param options Configuration options
   */
  TracerProvider(TracerProviderOptions options) : trace::TracerProvider()
  {
    // By default we ensure that all events carry their with TraceId and SpanId
    GetOption(options, "enableTraceId", config_.enableTraceId, true);
    GetOption(options, "enableSpanId", config_.enableSpanId, true);

    // Backwards-compatibility option that allows to reuse ETW-specific parenting described here:
    // https://docs.microsoft.com/en-us/uwp/api/windows.foundation.diagnostics.loggingoptions.relatedactivityid
    // https://docs.microsoft.com/en-us/windows/win32/api/evntprov/nf-evntprov-eventwritetransfer

    // Map current `SpanId` to ActivityId - GUID that uniquely identifies this activity. If NULL,
    // ETW gets the identifier from the thread local storage. For details on getting this
    // identifier, see EventActivityIdControl.
    GetOption(options, "enableActivityId", config_.enableActivityId, false);

    // Map parent `SpanId` to RelatedActivityId -  Activity identifier from the previous component.
    // Use this parameter to link your component's events to the previous component's events.
    GetOption(options, "enableRelatedActivityId", config_.enableRelatedActivityId, false);

    // When a new Span is started, the current span automatically becomes its parent.
    GetOption(options, "enableAutoParent", config_.enableAutoParent, false);
  }

  TracerProvider() : trace::TracerProvider()
  {
    config_.enableTraceId           = true;
    config_.enableSpanId            = true;
    config_.enableActivityId        = false;
    config_.enableRelatedActivityId = false;
    config_.enableAutoParent        = false;
  }

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
