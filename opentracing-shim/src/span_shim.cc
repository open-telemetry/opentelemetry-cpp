/*
 * Copyright The OpenTelemetry Authors
 * SPDX-License-Identifier: Apache-2.0
 */

#include "span_shim.h"
#include "span_context_shim.h"
#include "tracer_shim.h"
#include "shim_utils.h"

#include "opentelemetry/trace/semantic_conventions.h"
#include "opentelemetry/trace/span_metadata.h"
#include <opentracing/ext/tags.h>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace opentracingshim 
{

void SpanShim::handleError(const opentracing::Value& value) noexcept
{
  using opentelemetry::trace::StatusCode;
    
  auto code = StatusCode::kUnset;
  const auto& str_value = shimutils::stringFromValue(value);
  if (str_value == "true") 
  {
    code = StatusCode::kError;
  }
  else if (str_value == "false") 
  {
    code = StatusCode::kOk;
  }

  span_->SetStatus(code);
} 

void SpanShim::FinishWithOptions(const opentracing::FinishSpanOptions& finish_span_options) noexcept
{
  span_->End({{ finish_span_options.finish_steady_timestamp }});
}

void SpanShim::SetOperationName(opentracing::string_view name) noexcept
{
  span_->UpdateName(name.data());
}

void SpanShim::SetTag(opentracing::string_view key, const opentracing::Value& value) noexcept
{
  if (key == opentracing::ext::error)
  {
    handleError(value);
  }
  else
  {
    span_->SetAttribute(key.data(), shimutils::attributeFromValue(value));
  }
}

void SpanShim::SetBaggageItem(opentracing::string_view restricted_key, opentracing::string_view value) noexcept
{
  const std::lock_guard<decltype(context_lock_)> guard(context_lock_);
  context_ = context_.newWithKeyValue(restricted_key.data(), value.data());
}

std::string SpanShim::BaggageItem(opentracing::string_view restricted_key) const noexcept
{
  const std::lock_guard<decltype(context_lock_)> guard(context_lock_);
  std::string value;
  return context_.BaggageItem(restricted_key.data(), value) ? value : "";
}

void SpanShim::Log(std::initializer_list<EventEntry> fields) noexcept
{
  logImpl(opentracing::SystemTime::min(), fields);
}

void SpanShim::Log(opentracing::SystemTime timestamp, std::initializer_list<EventEntry> fields) noexcept
{
  logImpl(timestamp, fields);
}

void SpanShim::Log(opentracing::SystemTime timestamp, const std::vector<EventEntry>& fields) noexcept
{
  logImpl(timestamp, fields);
}

void SpanShim::logImpl(opentracing::SystemTime timestamp, nostd::span<const EventEntry> fields) noexcept 
{
  const auto event = std::find_if(fields.begin(), fields.end(), [](EventEntry item){ return item.first == "event"; });  
  auto name = (event != fields.end()) ? shimutils::stringFromValue(event->second) : std::string{"log"};

  bool is_error = (name == opentracing::ext::error);
  if (is_error) name = "exception";

  std::vector<std::pair<nostd::string_view, common::AttributeValue>> attributes;
  attributes.reserve(fields.size());
  
  for (const auto& entry : fields) 
  {
    auto key = entry.first;
    const auto& value = shimutils::attributeFromValue(entry.second);

    if (is_error) 
    {
      if (key == "error.kind") 
      {
        key = opentelemetry::trace::SemanticConventions::kExceptionType;
      } 
      else if (key == "message") 
      {
        key = opentelemetry::trace::SemanticConventions::kExceptionMessage;
      } 
      else if (key == "stack") 
      {
        key = opentelemetry::trace::SemanticConventions::kExceptionStacktrace;
      }
    }
    
    attributes.emplace_back(key, value);
  }

  if (timestamp != opentracing::SystemTime::min()) 
  {
    span_->AddEvent(name, timestamp, attributes);
  } 
  else 
  {
    span_->AddEvent(name, attributes);
  }
}

} // namespace opentracingshim
OPENTELEMETRY_END_NAMESPACE