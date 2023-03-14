/*
 * Copyright The OpenTelemetry Authors
 * SPDX-License-Identifier: Apache-2.0
 */

#include "opentelemetry/opentracingshim/span_shim.h"
#include "opentelemetry/opentracingshim/shim_utils.h"
#include "opentelemetry/opentracingshim/span_context_shim.h"
#include "opentelemetry/opentracingshim/tracer_shim.h"

#include "opentelemetry/trace/semantic_conventions.h"
#include "opentelemetry/trace/span_metadata.h"
#include "opentracing/ext/tags.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace opentracingshim
{

void SpanShim::handleError(const opentracing::Value &value) noexcept
{
  using opentelemetry::trace::StatusCode;
  // The error tag MUST be mapped to StatusCode:
  const auto &error_tag = utils::stringFromValue(value);
  // - no value being set maps to Unset.
  auto code = StatusCode::kUnset;

  if (error_tag == "true")  // - true maps to Error.
  {
    code = StatusCode::kError;
  }
  else if (error_tag == "false")  // - false maps to Ok.
  {
    code = StatusCode::kOk;
  }

  span_->SetStatus(code);
}

void SpanShim::FinishWithOptions(const opentracing::FinishSpanOptions &finish_span_options) noexcept
{
  // If an explicit timestamp is specified, a conversion MUST
  // be done to match the OpenTracing and OpenTelemetry units.
  span_->End({{finish_span_options.finish_steady_timestamp}});
}

void SpanShim::SetOperationName(opentracing::string_view name) noexcept
{
  span_->UpdateName(name.data());
}

void SpanShim::SetTag(opentracing::string_view key, const opentracing::Value &value) noexcept
{
  // Calls Set Attribute on the underlying OpenTelemetry Span with the specified key/value pair.
  if (key == opentracing::ext::error)
  {
    handleError(value);
  }
  else
  {
    span_->SetAttribute(key.data(), utils::attributeFromValue(value));
  }
}

void SpanShim::SetBaggageItem(opentracing::string_view restricted_key,
                              opentracing::string_view value) noexcept
{
  // Creates a new SpanContext Shim with a new OpenTelemetry Baggage containing the specified
  // Baggage key/value pair, and sets it as the current instance for this Span Shim.
  if (restricted_key.empty() || value.empty())
    return;
  // This operation MUST be safe to be called concurrently.
  const std::lock_guard<decltype(context_lock_)> guard(context_lock_);
  context_ = context_.newWithKeyValue(restricted_key.data(), value.data());
}

std::string SpanShim::BaggageItem(opentracing::string_view restricted_key) const noexcept
{
  // Returns the value for the specified key in the OpenTelemetry Baggage
  // of the current SpanContext Shim, or null if none exists.
  if (restricted_key.empty())
    return "";
  // This operation MUST be safe to be called concurrently.
  const std::lock_guard<decltype(context_lock_)> guard(context_lock_);
  std::string value;
  return context_.BaggageItem(restricted_key.data(), value) ? value : "";
}

void SpanShim::Log(std::initializer_list<EventEntry> fields) noexcept
{
  // If an explicit timestamp is specified, a conversion MUST
  // be done to match the OpenTracing and OpenTelemetry units.
  logImpl(fields, nullptr);
}

void SpanShim::Log(opentracing::SystemTime timestamp,
                   std::initializer_list<EventEntry> fields) noexcept
{
  // If an explicit timestamp is specified, a conversion MUST
  // be done to match the OpenTracing and OpenTelemetry units.
  logImpl(fields, &timestamp);
}

void SpanShim::Log(opentracing::SystemTime timestamp,
                   const std::vector<EventEntry> &fields) noexcept
{
  // If an explicit timestamp is specified, a conversion MUST
  // be done to match the OpenTracing and OpenTelemetry units.
  logImpl(fields, &timestamp);
}

void SpanShim::logImpl(nostd::span<const EventEntry> fields,
                       const opentracing::SystemTime *const timestamp) noexcept
{
  // The Add Event’s name parameter MUST be the value with the event key
  // in the pair set, or else fallback to use the log literal string.
  const auto &event = std::find_if(fields.begin(), fields.end(),
                                   [](const EventEntry &item) { return item.first == "event"; });
  auto name = (event != fields.end()) ? utils::stringFromValue(event->second) : std::string{"log"};
  // If pair set contains an event=error entry, the values MUST be mapped to an Event
  // with the conventions outlined in the Exception semantic conventions document:
  bool is_error = (name == opentracing::ext::error);
  // A call to AddEvent is performed with name being set to exception
  if (is_error)
    name = "exception";
  // Along the specified key/value pair set as additional event attributes...
  std::vector<std::pair<nostd::string_view, common::AttributeValue>> attributes;
  attributes.reserve(fields.size());

  for (const auto &entry : fields)
  {
    nostd::string_view key = entry.first.data();
    // ... including mapping of the following key/value pairs:
    if (is_error)
    {
      if (key == "error.kind")  // - error.kind maps to exception.type.
      {
        key = opentelemetry::trace::SemanticConventions::kExceptionType;
      }
      else if (key == "message")  // - message maps to exception.message.
      {
        key = opentelemetry::trace::SemanticConventions::kExceptionMessage;
      }
      else if (key == "stack")  // - stack maps to exception.stacktrace.
      {
        key = opentelemetry::trace::SemanticConventions::kExceptionStacktrace;
      }
    }

    attributes.emplace_back(key, utils::attributeFromValue(entry.second));
  }

  // Calls Add Events on the underlying OpenTelemetry Span with the specified key/value pair set.
  if (timestamp)
  {
    span_->AddEvent(name, *timestamp, attributes);
  }
  else
  {
    span_->AddEvent(name, attributes);
  }
}

}  // namespace opentracingshim
OPENTELEMETRY_END_NAMESPACE
