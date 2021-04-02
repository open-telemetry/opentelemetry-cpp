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

#include "opentelemetry/exporters/etw/etw_fields.h"
#include "opentelemetry/exporters/etw/etw_properties.h"
#include "opentelemetry/exporters/etw/etw_provider.h"
#include "opentelemetry/exporters/etw/utils.h"

namespace core  = opentelemetry::core;
namespace trace = opentelemetry::trace;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace etw
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
  bool enableActivityTracking;   // Emit TraceLogging events for Span/Start and Span/Stop
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

/**
 * @brief Template that allows to instantiate new Span object for header-only forward-declared
 * etw::Span type
 *
 * @tparam SpanType     Expected to be etw::Span
 * @tparam TracerType   expected to be etw::Tracer
 * @param objPtr        Pointer to parent
 * @param name          Span Name
 * @param options       Span Options
 * @return              Span instance
 */
template <class SpanType, class TracerType>
SpanType *new_span(TracerType *objPtr,
                   nostd::string_view name,
                   const trace::StartSpanOptions &options)
{
  return new (std::nothrow) SpanType{*objPtr, name, options};
}

/**
 * @brief Template that allows to convert etw::Span pointer to smart shared pointer to `trace::Span`
 * @tparam SpanType     Expected to be etw::Span
 * @param ptr           Pointer to etw::Span
 * @return              Smart shared pointer to `trace::Span`
 */
template <class SpanType>
nostd::shared_ptr<trace::Span> to_span_ptr(SpanType *ptr)
{
  return nostd::shared_ptr<trace::Span>{ptr};
}

class TracerProvider;

/**
 * @brief Utility template for obtaining Span Name
 * @tparam T            etw::Span
 * @param t             instance of etw::Span
 * @return              Span Name
 */
template <class T>
std::string GetName(T &t)
{
  auto sV = t.GetName();
  return std::string(sV.data(), sV.length());
}

/**
 * @brief Utility template to obtain Span start time
 * @tparam T            etw::Span
 * @param t             instance of etw::Span
 * @return              Span Start timestamp
 */
template <class T>
core::SystemTimestamp GetStartTime(T &t)
{
  return t.GetStartTime();
}

/**
 * @brief Utility template to obtain Span end time
 * @tparam T           etw::Span
 * @param t            instance of etw::Span
 * @return             Span Stop timestamp
 */
template <class T>
core::SystemTimestamp GetEndTime(T &t)
{
  return t.GetEndTime();
}

class Properties;

/**
 * @brief Utility template to store Attributes on Span
 * @tparam T           etw::Span
 * @param instance     instance of etw::Span
 * @param t            Properties to store as Attributes
 */
template <class T>
void SetSpanAttributes(T &instance, Properties &t)
{
  instance.SetAttributes(t);
}

/**
 * @brief Utility template to obtain Span Attributes
 * @tparam T           etw::Span
 * @param instance     instance of etw::Span
 * @return             ref to Span Attributes
 */
template <class T>
Properties &GetSpanAttributes(T &instance)
{
  return instance.GetAttributes();
}

/**
 * @brief Utility template to obtain etw::TracerProvider._config
 *
 * @tparam T    etw::TracerProvider
 * @param t     etw::TracerProvider ref
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
  etw::TracerProvider &tracerProvider_;

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
   * @brief Allow our friendly etw::Span to end itself on Tracer.
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

    // Populate Span with presaved attributes
    Span &currentSpan   = const_cast<Span &>(span);
    Properties evt      = GetSpanAttributes(currentSpan);
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

    if (cfg.enableActivityTracking)
    {
      // TODO: check what EndSpanOptions should be supported for this exporter.
      // The only option available currently (end_steady_time) does not apply.
      //
      // This event on Span Stop enables generation of "non-transactional"
      // OpCode=Stop in alignment with TraceLogging Activity "EventSource"
      // spec.
      etwProvider().write(provHandle, evt, ActivityIdPtr, RelatedActivityIdPtr, 2, encoding);
    }

    {
      // Now since the span has ended, we need to emit the "Span" event that
      // contains the entire span information, attributes, time, etc. on it.
      evt[ETW_FIELD_NAME]         = ETW_VALUE_SPAN;
      evt[ETW_FIELD_PAYLOAD_NAME] = GetName(span);

      // Add timing details in ISO8601 format, which adequately represents
      // the actual time, taking Timezone into consideration. This is NOT
      // local time, but rather UTC time (Z=0).
      std::chrono::system_clock::time_point startTime = GetStartTime(currentSpan);
      std::chrono::system_clock::time_point endTime   = GetEndTime(currentSpan);
      int64_t startTimeMs =
          std::chrono::duration_cast<std::chrono::milliseconds>(startTime.time_since_epoch())
              .count();
      int64_t endTimeMs =
          std::chrono::duration_cast<std::chrono::milliseconds>(endTime.time_since_epoch()).count();

      // It may be more optimal to enable passing timestamps as UTC milliseconds
      // since Unix epoch instead of string, but that implies additional tooling
      // is needed to convert it, rendering it NOT human-readable.
      evt[ETW_FIELD_STARTTIME] = utils::formatUtcTimestampMsAsISO8601(startTimeMs);

      // Duration of Span in milliseconds
      evt[ETW_FIELD_DURATION] = endTimeMs - startTimeMs;
      etwProvider().write(provHandle, evt, ActivityIdPtr, RelatedActivityIdPtr, 0, encoding);
    }

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
   * @brief Init a reference to etw::ProviderHandle
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
  Tracer(etw::TracerProvider &parent,
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
    // Populate TraceId of the Tracer with the above GUID
    const auto *traceIdPtr = reinterpret_cast<const uint8_t *>(std::addressof(trace_id));
    nostd::span<const uint8_t, trace::TraceId::kSize> traceIdBytes(
        traceIdPtr, traceIdPtr + trace::TraceId::kSize);
    traceId_ = trace::TraceId(traceIdBytes);
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
#ifdef RTTI_ENABLED
    common::KeyValueIterable &attribs = const_cast<common::KeyValueIterable &>(attributes);
    Properties *evt                   = dynamic_cast<Properties *>(&attribs);
    if (evt != nullptr)
    {
      // Pass as a reference to original modifyable collection without creating a copy
      return StartSpan(name, *evt, links, options);
    }
#endif
    Properties evtCopy = attributes;
    return StartSpan(name, evtCopy, links, options);
  }

  /**
   * @brief Start Span
   * @param name Span name
   * @param attributes Span attributes
   * @param links Span links
   * @param options Span options
   * @return
   */
  virtual nostd::shared_ptr<trace::Span> StartSpan(
      nostd::string_view name,
      Properties &evt,
      const trace::SpanContextKeyValueIterable &links,
      const trace::StartSpanOptions &options = {}) noexcept
  {
    const auto &cfg = GetConfiguration(tracerProvider_);

    // Parent Context:
    // - either use current span
    // - or attach to parent SpanContext specified in options
    const auto parentContext =
        (options.parent.IsValid()) ? options.parent : GetCurrentSpan()->GetContext();

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

    // This template pattern allows us to forward-declare the etw::Span,
    // create an instance of it, then assign it to tracer::Span result.
    auto currentSpan                      = new_span<Span, Tracer>(this, name, options);
    nostd::shared_ptr<trace::Span> result = to_span_ptr<Span>(currentSpan);

    auto spanContext = result->GetContext();

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

    // Remember Span attributes to be passed down to ETW on Span end
    SetSpanAttributes(*currentSpan, evt);

    if (cfg.enableActivityTracking)
    {
      // TODO: add support for options that are presently ignored :
      // - options.kind
      // - options.start_steady_time
      // - options.start_system_time
      etwProvider().write(provHandle, evt, ActivityIdPtr, RelatedActivityIdPtr, 1, encoding);
    };

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
#ifdef RTTI_ENABLED
    common::KeyValueIterable &attribs = const_cast<common::KeyValueIterable &>(attributes);
    Properties *evt                   = dynamic_cast<Properties *>(&attribs);
    if (evt != nullptr)
    {
      // Pass as a reference to original modifyable collection without creating a copy
      return AddEvent(span, name, timestamp, *evt);
    }
#endif
    // Pass a copy converted to Properties object on stack
    Properties evtCopy = attributes;
    return AddEvent(span, name, timestamp, evtCopy);
  }

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
                Properties &evt) noexcept
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
 * @brief etw::Span allows to send event data to ETW listener.
 */
class Span : public trace::Span
{
protected:
  friend class Tracer;

  /**
   * @brief Span properties are attached on "Span" event on end of Span.
   */
  Properties attributes_;

  core::SystemTimestamp start_time_;
  core::SystemTimestamp end_time_;

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
    const auto *activityIdPtr = reinterpret_cast<const uint8_t *>(std::addressof(activity_id));

    // Populate SpanId with that GUID
    nostd::span<const uint8_t, trace::SpanId::kSize> spanIdBytes(
        activityIdPtr, activityIdPtr + trace::SpanId::kSize);
    const trace::SpanId spanId(spanIdBytes);

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
   * @brief Get start time of this Span.
   * @return
   */
  core::SystemTimestamp GetStartTime() { return start_time_; }

  /**
   * @brief Get end time of this Span.
   * @return
   */
  core::SystemTimestamp GetEndTime() { return end_time_; }

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
      : trace::Span(),
        owner_(owner),
        parent_(parent),
        context_(CreateContext()),
        start_time_(std::chrono::system_clock::now())
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
  }

  void SetAttributes(Properties attributes) { attributes_ = attributes; }

  /**
   * @brief Obtain span attributes specified at Span start.
   * NOTE: please consider that this method is NOT thread-safe.
   *
   * @return ref to Properties collection
   */
  Properties &GetAttributes() { return attributes_; }

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
    end_time_ = std::chrono::system_clock::now();

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

    // Emit separate events compatible with TraceLogging Activity/Start and Activity/Stop
    // format for every Span emitted.
    GetOption(options, "enableActivityTracking", config_.enableActivityTracking, false);

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
    config_.enableActivityTracking  = false;
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

}  // namespace etw
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
