// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

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

#include "opentelemetry/exporters/etw/etw_config.h"
#include "opentelemetry/exporters/etw/etw_fields.h"
#include "opentelemetry/exporters/etw/etw_properties.h"
#include "opentelemetry/exporters/etw/etw_provider.h"
#include "opentelemetry/exporters/etw/utils.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace etw
{

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
                   const opentelemetry::trace::StartSpanOptions &options)
{
  return new (std::nothrow) SpanType{*objPtr, name, options};
}

/**
 * @brief Template that allows to convert etw::Span pointer to smart shared pointer to
 * `opentelemetry::trace::Span`
 * @tparam SpanType     Expected to be etw::Span
 * @param ptr           Pointer to etw::Span
 * @return              Smart shared pointer to `opentelemetry::trace::Span`
 */
template <class SpanType>
nostd::shared_ptr<opentelemetry::trace::Span> to_span_ptr(SpanType *ptr)
{
  return nostd::shared_ptr<opentelemetry::trace::Span>{ptr};
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
common::SystemTimestamp GetStartTime(T &t)
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
common::SystemTimestamp GetEndTime(T &t)
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

template <class T>
void UpdateStatus(T &t, Properties &props)
{
  t.UpdateStatus(props);
}

/**
 * @brief Tracer class that allows to send spans to ETW Provider.
 */
class Tracer : public opentelemetry::trace::Tracer
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

  opentelemetry::trace::TraceId traceId_;

  std::atomic<bool> isClosed_{true};

  /**
   * @brief ETWProvider is a singleton that aggregates all ETW writes.
   * @return
   */
  static ETWProvider &etwProvider()
  {
    static ETWProvider instance;  // C++11 magic static
    return instance;
  }

  /**
   * @brief Internal method that allows to populate Links to other Spans.
   * Span links are in hexadecimal representation, comma-separated in their
   * order of appearance.
   *
   * @param attributes
   * @param links
   */
  virtual void DecorateLinks(Properties &attributes,
                             const opentelemetry::trace::SpanContextKeyValueIterable &links) const
  {
    // Add `SpanLinks` attribute if the list is not empty
    if (links.size())
    {
      size_t idx = 0;
      std::string linksValue;
      links.ForEachKeyValue(
          [&](opentelemetry::trace::SpanContext ctx, const common::KeyValueIterable &) {
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
  }

  /**
   * @brief Allow our friendly etw::Span to end itself on Tracer.
   * @param span
   * @param
   */
  virtual void EndSpan(const Span &span,
                       const opentelemetry::trace::Span *parentSpan = nullptr,
                       const opentelemetry::trace::EndSpanOptions & = {})
  {
    const auto &cfg = GetConfiguration(tracerProvider_);
    const opentelemetry::trace::Span &spanBase =
        reinterpret_cast<const opentelemetry::trace::Span &>(span);
    auto spanContext = spanBase.GetContext();

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
      if (CopySpanIdToActivityId(spanBase.GetContext().span_id(), ActivityId))
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
        if (CopySpanIdToActivityId(parentSpan->GetContext().span_id(), RelatedActivityId))
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
#ifdef ETW_FIELD_ENDTTIME
      // ETW has its own precise timestamp at envelope layer for every event.
      // However, in some scenarios it is easier to deal with ISO8601 strings.
      // In that case we convert the app-created timestamp and place it into
      // Payload[$ETW_FIELD_TIME] field. The option configurable at compile-time.
      evt[ETW_FIELD_ENDTTIME] = utils::formatUtcTimestampMsAsISO8601(endTimeMs);
#endif
      // Duration of Span in milliseconds
      evt[ETW_FIELD_DURATION] = endTimeMs - startTimeMs;
      // Presently we assume that all spans are server spans
      evt[ETW_FIELD_SPAN_KIND] = uint32_t(opentelemetry::trace::SpanKind::kServer);
      UpdateStatus(currentSpan, evt);
      etwProvider().write(provHandle, evt, ActivityIdPtr, RelatedActivityIdPtr, 0, encoding);
    }
  }

  const opentelemetry::trace::TraceId &trace_id() { return traceId_; }

  friend class Span;

  /**
   * @brief Init a reference to etw::ProviderHandle
   * @return Provider Handle
   */
  ETWProvider::Handle &initProvHandle()
  {
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
      : opentelemetry::trace::Tracer(),
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
    nostd::span<const uint8_t, opentelemetry::trace::TraceId::kSize> traceIdBytes(
        traceIdPtr, traceIdPtr + opentelemetry::trace::TraceId::kSize);
    traceId_ = opentelemetry::trace::TraceId(traceIdBytes);
  }

  /**
   * @brief Start Span
   * @param name Span name
   * @param attributes Span attributes
   * @param links Span links
   * @param options Span options
   * @return
   */
  nostd::shared_ptr<opentelemetry::trace::Span> StartSpan(
      nostd::string_view name,
      const common::KeyValueIterable &attributes,
      const opentelemetry::trace::SpanContextKeyValueIterable &links,
      const opentelemetry::trace::StartSpanOptions &options = {}) noexcept override
  {
#ifdef OPENTELEMETRY_RTTI_ENABLED
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
  virtual nostd::shared_ptr<opentelemetry::trace::Span> StartSpan(
      nostd::string_view name,
      Properties &evt,
      const opentelemetry::trace::SpanContextKeyValueIterable &links,
      const opentelemetry::trace::StartSpanOptions &options = {}) noexcept
  {
    const auto &cfg = GetConfiguration(tracerProvider_);

    // Parent Context:
    // - either use current span
    // - or attach to parent SpanContext specified in options
    opentelemetry::trace::SpanContext parentContext = GetCurrentSpan()->GetContext();
    if (nostd::holds_alternative<opentelemetry::trace::SpanContext>(options.parent))
    {
      auto span_context = nostd::get<opentelemetry::trace::SpanContext>(options.parent);
      if (span_context.IsValid())
      {
        parentContext = span_context;
      }
    }

    // Populate Etw.RelatedActivityId at envelope level if enabled
    GUID RelatedActivityId;
    LPCGUID RelatedActivityIdPtr = nullptr;
    if (cfg.enableAutoParent)
    {
      if (cfg.enableRelatedActivityId)
      {
        if (CopySpanIdToActivityId(parentContext.span_id(), RelatedActivityId))
        {
          RelatedActivityIdPtr = &RelatedActivityId;
        }
      }
    }

    // This template pattern allows us to forward-declare the etw::Span,
    // create an instance of it, then assign it to tracer::Span result.
    auto currentSpan = new_span<Span, Tracer>(this, name, options);
    nostd::shared_ptr<opentelemetry::trace::Span> result = to_span_ptr<Span>(currentSpan);

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
      if (CopySpanIdToActivityId(result.get()->GetContext().span_id(), ActivityId))
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
    }

    return result;
  }

  /**
   * @brief Force flush data to Tracer, spending up to given amount of microseconds to flush.
   * NOTE: this method has no effect for the realtime streaming Tracer.
   *
   * @param timeout Allow Tracer to drop data if timeout is reached
   * @return
   */
  void ForceFlushWithMicroseconds(uint64_t) noexcept override {}

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
  }

  /**
   * @brief Add event data to span associated with tracer.
   * @param span Parent span.
   * @param name Event name.
   * @param timestamp Event timestamp.
   * @param attributes Event attributes.
   * @return
   */
  void AddEvent(opentelemetry::trace::Span &span,
                nostd::string_view name,
                common::SystemTimestamp timestamp,
                const common::KeyValueIterable &attributes) noexcept
  {
#ifdef OPENTELEMETRY_RTTI_ENABLED
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
  void AddEvent(opentelemetry::trace::Span &span,
                nostd::string_view name,
                common::SystemTimestamp timestamp,
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
      if (CopySpanIdToActivityId(spanContext.span_id(), ActivityId))
      {
        ActivityIdPtr = &ActivityId;
      }
    }

#ifdef HAVE_FIELD_TIME
    {
      auto timeNow        = std::chrono::system_clock::now().time_since_epoch();
      auto millis         = std::chrono::duration_cast<std::chrono::milliseconds>(timeNow).count();
      evt[ETW_FIELD_TIME] = utils::formatUtcTimestampMsAsISO8601(millis);
    }
#endif

    etwProvider().write(provHandle, evt, ActivityIdPtr, nullptr, 0, encoding);
  }

  /**
   * @brief Add event data to span associated with tracer.
   * @param span Span.
   * @param name Event name.
   * @param timestamp Event timestamp.
   * @return
   */
  void AddEvent(opentelemetry::trace::Span &span,
                nostd::string_view name,
                common::SystemTimestamp timestamp) noexcept
  {
    AddEvent(span, name, timestamp, sdk::GetEmptyAttributes());
  }

  /**
   * @brief Add event data to span associated with tracer.
   * @param span Spab.
   * @param name Event name.
   */
  void AddEvent(opentelemetry::trace::Span &span, nostd::string_view name)
  {
    AddEvent(span, name, std::chrono::system_clock::now(), sdk::GetEmptyAttributes());
  }

  /**
   * @brief Tracer destructor.
   */
  virtual ~Tracer() { CloseWithMicroseconds(0); }
};

/**
 * @brief etw::Span allows to send event data to ETW listener.
 */
class Span : public opentelemetry::trace::Span
{
protected:
  friend class Tracer;

  /**
   * @brief Span properties are attached on "Span" event on end of Span.
   */
  Properties attributes_;

  common::SystemTimestamp start_time_;
  common::SystemTimestamp end_time_;

  opentelemetry::trace::StatusCode status_code_{opentelemetry::trace::StatusCode::kUnset};
  std::string status_description_;

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

  opentelemetry::trace::SpanContext context_;

  const opentelemetry::trace::SpanContext CreateContext()
  {
    GUID activity_id;
    // Generate random GUID
    CoCreateGuid(&activity_id);
    const auto *activityIdPtr = reinterpret_cast<const uint8_t *>(std::addressof(activity_id));

    // Populate SpanId with that GUID
    nostd::span<const uint8_t, opentelemetry::trace::SpanId::kSize> spanIdBytes(
        activityIdPtr, activityIdPtr + opentelemetry::trace::SpanId::kSize);
    const opentelemetry::trace::SpanId spanId(spanIdBytes);

    // Inherit trace_id from Tracer
    const opentelemetry::trace::TraceId traceId{owner_.trace_id()};
    // TODO: TraceFlags are not supported by ETW exporter.
    const opentelemetry::trace::TraceFlags flags{0};
    // TODO: Remote parent is not supported by ETW exporter.
    const bool hasRemoteParent = false;
    return opentelemetry::trace::SpanContext{traceId, spanId, flags, hasRemoteParent};
  }

public:
  /**
   * @brief Update Properties object with current Span status
   * @param evt
   */
  void UpdateStatus(Properties &evt)
  {
    /* Should we avoid populating this extra field if status is unset? */
    if ((status_code_ == opentelemetry::trace::StatusCode::kUnset) ||
        (status_code_ == opentelemetry::trace::StatusCode::kOk))
    {
      evt[ETW_FIELD_SUCCESS]       = "True";
      evt[ETW_FIELD_STATUSCODE]    = uint32_t(status_code_);
      evt[ETW_FIELD_STATUSMESSAGE] = status_description_;
    }
    else
    {
      evt[ETW_FIELD_SUCCESS]       = "False";
      evt[ETW_FIELD_STATUSCODE]    = uint32_t(status_code_);
      evt[ETW_FIELD_STATUSMESSAGE] = status_description_;
    }
  }

  /**
   * @brief Get start time of this Span.
   * @return
   */
  common::SystemTimestamp GetStartTime() { return start_time_; }

  /**
   * @brief Get end time of this Span.
   * @return
   */
  common::SystemTimestamp GetEndTime() { return end_time_; }

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
       const opentelemetry::trace::StartSpanOptions &options,
       Span *parent = nullptr) noexcept
      : opentelemetry::trace::Span(),
        start_time_(std::chrono::system_clock::now()),
        owner_(owner),
        parent_(parent),
        context_(CreateContext())
  {
    name_ = name;
    UNREFERENCED_PARAMETER(options);
  }

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
  void AddEvent(nostd::string_view name, common::SystemTimestamp timestamp) noexcept override
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
                common::SystemTimestamp timestamp,
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
  void SetStatus(opentelemetry::trace::StatusCode code,
                 nostd::string_view description) noexcept override
  {
    status_code_        = code;
    status_description_ = description.data();
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
    // don't override fields propagated from span data.
    if (key == ETW_FIELD_NAME || key == ETW_FIELD_SPAN_ID || key == ETW_FIELD_TRACE_ID)
    {
      return;
    }
    attributes_[std::string{key}].FromAttributeValue(value);
  }

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
  void End(const opentelemetry::trace::EndSpanOptions &options = {}) noexcept override
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
  opentelemetry::trace::SpanContext GetContext() const noexcept override { return context_; }

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
  opentelemetry::trace::Tracer &tracer() const noexcept { return this->owner_; }
};

/**
 * @brief ETW TracerProvider
 */
class TracerProvider : public opentelemetry::trace::TracerProvider
{
public:
  /**
   * @brief TracerProvider options supplied during initialization.
   */
  TelemetryProviderConfiguration config_;

  /**
   * @brief Construct instance of TracerProvider with given options
   * @param options Configuration options
   */
  TracerProvider(TelemetryProviderOptions options) : opentelemetry::trace::TracerProvider()
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

    // Map parent `SpanId` to RelatedActivityId -  Activity identifier from the previous
    // component. Use this parameter to link your component's events to the previous component's
    // events.
    GetOption(options, "enableRelatedActivityId", config_.enableRelatedActivityId, false);

    // When a new Span is started, the current span automatically becomes its parent.
    GetOption(options, "enableAutoParent", config_.enableAutoParent, false);

    // Determines what encoding to use for ETW events: TraceLogging Dynamic, MsgPack, XML, etc.
    config_.encoding = GetEncoding(options);
  }

  TracerProvider() : opentelemetry::trace::TracerProvider()
  {
    config_.enableTraceId           = true;
    config_.enableSpanId            = true;
    config_.enableActivityId        = false;
    config_.enableActivityTracking  = false;
    config_.enableRelatedActivityId = false;
    config_.enableAutoParent        = false;
    config_.encoding                = ETWProvider::EventFormat::ETW_MANIFEST;
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
  nostd::shared_ptr<opentelemetry::trace::Tracer> GetTracer(
      nostd::string_view name,
      nostd::string_view args       = "",
      nostd::string_view schema_url = "") noexcept override
  {
    UNREFERENCED_PARAMETER(args);
    UNREFERENCED_PARAMETER(schema_url);
    ETWProvider::EventFormat evtFmt = config_.encoding;
    return nostd::shared_ptr<opentelemetry::trace::Tracer>{new (std::nothrow)
                                                               Tracer(*this, name, evtFmt)};
  }
};

}  // namespace etw
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
